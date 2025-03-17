/*
 *     Name: main.cpp
 *
 *     Description: process1: Tokens example.
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

#ifdef WIN32
#include <windows.h>
#define l_sleep(a) Sleep(a * 1000)
#else
#include <unistd.h>
#define l_sleep(a) sleep(a)
#endif
#include <string>

#include <boost/format.hpp>
#include <boost/program_options.hpp>
#include <isl_api.h>

#include "swversion.h"


/*
 *     Macros and constants definition
 */

namespace bpo = boost::program_options;
const char c_Process[] = "process1";


/*
 *     Types definition
 */

typedef struct {
	bool m_bWithDelay;
} tCmdLine;


/*
 *     Local functions
 */

static bool GetCmdLine(int argc, char** argv, tCmdLine * stCmdLine)
{
	if (stCmdLine == NULL) {
		return false;
	}
	bpo::options_description bpDesc("Allowed options");
	bpDesc.add_options()
		("version,v", "print version number")
		("help,h", "print help message")
		("withdelay,d", "A delay is used on data exchange");
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
		ISLLogError(300, "Command line error: %s", osMsg.str().c_str());
		return false;
	}
	// Help
	if (bpVars.count("help")) {
		std::ostringstream osMsg;
		osMsg << bpDesc;
		ISLLogInfo(100, "Command line description:\n%s", osMsg.str().c_str());
		return false; // No need to go further
	}
	// Print version
	if (bpVars.count("version")) {
		ISLLogInfo(101, APP_NAME" version: " GET_APP_VERSION(FULL_VERSION_NUMBER));
		return false; // No need to go further
	}
	// Delay
	stCmdLine->m_bWithDelay = false;
	if (bpVars.count("withdelay")) {
		ISLLogInfo(107, "Delays will be simulated during the computation loop.");
		stCmdLine->m_bWithDelay = true;
	}
	return true;
}


/*
 *     Main function
 */

int main(int argc, char *argv[])
{
	//
	// Get the command line
	tCmdLine stCmdLine;
	if (GetCmdLine(argc, argv, &stCmdLine) == false)  {
		return -9;
	}
	//
	// Initialize ISL
	isl::CConnect cConnect;
	cConnect.CloseLogOnDelete(true);
	//
	// Loading the XML file
	if (cConnect.Load(boost::str(boost::format("../%1%.xml") % c_Process)) == false) {
		ISLLogError(301, "Failed to load the XML file: %s.", cConnect.GetFileName().c_str());
		return -1;
	}
	ISLLogInfo(103, "The file %s has been loaded.", cConnect.GetFileName().c_str());
	//
	// Session setup
	if (cConnect.Create() == false) {
		ISLLogError(303, "Failed to setup the OpenISL session %s.", cConnect.GetSessionId().c_str());
		return -3;
	}
	ISLLogInfo(105, "The session %s has been setup.", cConnect.GetSessionId().c_str());
	//
	// Data initialization
	int nLoops1 = 0;
	int nLoops2 = 0;
	double dToken1 = 1.0;
	double dToken2 = 1.0;
	isl::CData * cToken1W = cConnect.GetIO("Token1W");
	if (cToken1W == 0) {
		ISLLogError(304, "Unknown ISL variable Token1W.");
		return -4;
	}
	isl::CData * cToken1R = cConnect.GetIO("Token1R");
	if (cToken1R == 0) {
		ISLLogError(305, "Unknown ISL variable Token1R.");
		return -5;
	}
	isl::CData * cToken2W = cConnect.GetIO("Token2W");
	if (cToken2W == 0) {
		ISLLogError(306, "Unknown ISL variable Token2W.");
		return -6;
	}
	isl::CData * cToken2R = cConnect.GetIO("Token2R");
	if (cToken2R == 0) {
		ISLLogError(307, "Unknown ISL variable Token2R.");
		return -7;
	}
	ISLLogInfo(106, "All ISL variables have been mapped.");
	//
	// ISL connection
	if (cConnect.Connect(true) == false) {
		ISLLogError(309, "Failed to connect the inputs to ISL transmitters.");
		return -10;
	}
	ISLLogInfo(110, "Connected to the transmitters.");
	//
	// Get times
	double dTime = cConnect.GetStartTime();
	double dStepSize = cConnect.GetStepSize();
	double dEndTime = cConnect.GetEndTime();
	int nCondDelay = 0;
	while ((cConnect.IsTerminated() == false) && (dTime <= dEndTime)) {
		// Token1W
		if (cToken1W->SetData(&dToken1, dTime, true) == false) {
			ISLLogWarning(200, "Failed to set data on Token1W[%g].", dTime);
		}
		// Token2W
		if (cToken2W->SetData(&dToken2, dTime, true) == false) {
			ISLLogWarning(201, "Failed to set data on Token2W[%g].", dTime);
		}
		// Apply delay
		if (stCmdLine.m_bWithDelay) {
			if (nCondDelay % 2 == 0) {
				l_sleep(2);
			}
		}
		double dGivenTime;
		// Token1R
		if (cToken1R->GetData(&dToken1, &dGivenTime, dTime, true) == false) {
			ISLLogWarning(202, "Failed to get data on Token1R[%g].", dTime);
		}
		if ((int )round(dToken1) == 1) {
			nLoops1++;
			ISLLogInfo(108, "Token1: loop %d", nLoops1);
		}
		// Token2R
		if (cToken2R->GetData(&dToken2, &dGivenTime, dTime, true) == false) {
			ISLLogWarning(203, "Failed to get data on Token2R[%g].", dTime);
		}
		if ((int)round(dToken2) == 1) {
			nLoops2++;
			ISLLogInfo(109, "Token2: loop %d", nLoops2);
		}
		// Time increment
		dTime += dStepSize;
	}
	//
	// Closing the connection
	if (cConnect.Disconnect() == false) {
		ISLLogError(308, "Failed to disconnect from the ISL session.");
		return -8;
	}
	//
	//
	return 0;
}
