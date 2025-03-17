/*
 *     Name: main.cpp
 *
 *     Description: ISL Master FMI: Main program.
 *
 *     Author: T. Roudier
 *     Copyright (c) 2019-2025 E-Sim Solutions Inc
 *
 *     Distributed under the MIT License.
 * 
 *     --------------------------------------------------------------------------
 * 
 *     Permission is hereby granted, free of charge, to any person obtaining a
 *     copy of this software and associated documentation files (the “Software”),
 *     to deal in the Software without restriction, including without limitation
 *     the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *     and/or sell copies of the Software, and to permit persons to whom the
 *     Software is furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included in
 *     all copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *     DEALINGS IN THE SOFTWARE.
 * 
 *     --------------------------------------------------------------------------
 *
 */

/*
 *     Header files
 */

#include <string>
#include <signal.h>
#include <boost/program_options.hpp>
#include <isl_log.h>

#include "fmusim_const.h"
#include "swversion.h"
#include "fmusim.h"
#include "model.h"
#include "killerthread.h"

/*
 *     Macros and constants definition
 */

namespace bpo = boost::program_options;


/*
 *     Types definition
 */

typedef struct {
	std::string m_sInFile;
	std::string m_sSession;
	double m_dSpeed;
	bool m_bStop;
	bool m_bKeepDir;
	bool m_bUseWorkingDir;
	bool m_bUseLogger;
} tCmdLine;


/*
 *     Local functions
 */

static bool GetCmdLine(int argc, char** argv, tCmdLine * stCmdLine)
{
	if (stCmdLine == NULL) {
		return false;
	}
	stCmdLine->m_dSpeed = 0.0;
	stCmdLine->m_bStop = false;
	stCmdLine->m_bKeepDir = false;
	stCmdLine->m_bUseWorkingDir = false;
	stCmdLine->m_bUseLogger = false;
	bpo::options_description bpDesc("Allowed options");
	bpDesc.add_options()
		("version,v", "print version number")
		("help,h", "print help message")
		("input,i", bpo::value<std::string>(), "load a FMU file (." FMU_FILE_EXT ")")
		("id,d", bpo::value<std::string>(), "set the session identifier")
		("speed,x", bpo::value<double>(), "set the simulation speed")
		("stop,s", "send a stop request to the application")
		("local,l", "use the working directory")
		("logger,g", "use the FMU logger")
		("keep,k", "keep the temporary directory open");
	bpo::variables_map bpVars;
	try {
		bpo::store(bpo::parse_command_line(argc, argv, bpDesc), bpVars);
		bpo::notify(bpVars);
	}
	catch (bpo::error & bpErr)
	{
		std::ostringstream osMsg;
		osMsg << "Error: " << bpErr.what() << std::endl << std::endl;
		osMsg << bpDesc;
		AppLogError(ERROR_CMDLINE, "Command line error: %s", osMsg.str().c_str());
		return false;
	}
	// Help
	if (bpVars.count("help")) {
		std::ostringstream osMsg;
		osMsg << bpDesc;
		AppLogInfo(INFO_HELPMSG, "Command line description:\n%s", osMsg.str().c_str());
		return false; // No need to go further
	}
	// Print version
	if (bpVars.count("version")) {
		AppLogInfo(INFO_VERSION, " version: " GET_APP_VERSION(FULL_VERSION_NUMBER));
		return false; // No need to go further
	}
	// Stop option
	if (bpVars.count("stop")) {
		stCmdLine->m_bStop = true;
	}
	// Keep option
	if (bpVars.count("keep")) {
		stCmdLine->m_bKeepDir = true;
	}
	// Local option
	if (bpVars.count("local")) {
		stCmdLine->m_bUseWorkingDir = true;
	}
	// Logger option
	if (bpVars.count("logger")) {
		stCmdLine->m_bUseLogger = true;
	}
	// Id option
	if (bpVars.count("id")) {
		stCmdLine->m_sSession = bpVars["id"].as<std::string>();
	}
	// Speed option
	if (bpVars.count("speed")) {
		stCmdLine->m_dSpeed = bpVars["speed"].as<double>();
	}
	// Get the input file
	if (bpVars.count("input")) {
		stCmdLine->m_sInFile = bpVars["input"].as<std::string>();
	}
	return true;
}


/*
 *     Main function
 */

int main(int argc, char *argv[])
{
	// Log initialization
	AppLog->Init(DEFAULT_LOG_FILE_NAME);
	// Get the command line
	tCmdLine stCmdLine;
	if (GetCmdLine(argc, argv, &stCmdLine) == false)  {
		return -4;
	}
	if (stCmdLine.m_bStop) {
		if (CKillerThread::SendStopRequest() == false) {
			return -3;
		}
		return 0;
	}
	CModel * cModel = new CModel();
	cModel->SetKeepDir(stCmdLine.m_bKeepDir);
	cModel->SetUseWorkingDir(stCmdLine.m_bUseWorkingDir);
	cModel->SetUseLogger(stCmdLine.m_bUseLogger);
	if (stCmdLine.m_sInFile.empty() == false) {
		if (cModel->Load(stCmdLine.m_sInFile, false) == false) {
			return -2;
		}
	}
	if (stCmdLine.m_sSession.empty() == false) {
		cModel->SetSession(stCmdLine.m_sSession);
	}
	// Create the application
	static CFMUSim cApp(cModel);
	// Set the options
	cApp.SetSimulationRate(stCmdLine.m_dSpeed);
	// Manage how the application will be left
	static CKillerThread cKillThread(&cApp);
	cKillThread.Start();
	// Run the simulation
	if (cApp.Run() == false) {
		return -1;
	}
	cApp.Wait();
	return 0;
}
