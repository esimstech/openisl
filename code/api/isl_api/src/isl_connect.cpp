/*
 *     Name: isl_connect.h
 *
 *     Description: ISL connection element representing an instance of a couple simulator/model.
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

#include <algorithm>

#include <boost/filesystem.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/process/environment.hpp>

#include <isl_misc.h>
#include <isl_log.h>
#include <isl_xml.h>
#include <isl_shm.h>

#include "isl_api.h"
#include "isl_shm_connect.h"
#include "isl_exitthread.h"
#include "isl_settings.h"
#include "isl_errorcodes.h"


/*
 *     Classes definition
 */

std::string isl::CConnect::GetVersionNumber()
{
	return std::string(GET_APP_VERSION(VERSION_NUMBER));
}

std::string isl::CConnect::GenUID()
{
	return 	boost::lexical_cast<std::string>(boost::uuids::random_generator()());
}

bool isl::CConnect::StartThreadListenToStop(CConnect * cConnect)
{
	CExitThread * cThread = new CExitThread(cConnect, false);
	cThread->Start();
	return true;
}

bool isl::CConnect::SendStopRequest()
{
	return CExitThread::SendStopRequest();
}

bool isl::CConnect::SendStopSession(const std::string & sSession)
{
	return CExitThread::SendStopRequest(sSession);
}

isl::CConnect::CConnect()
{
	boost::filesystem::path bpLogFile(boost::filesystem::temp_directory_path());
	bpLogFile.append("isl_api.log");
	AppLog->Init(bpLogFile.string());
	m_uType = 0;
	m_ulPID = boost::this_process::get_id();
	m_nConnectTimeOut = 0;
	m_dStartTime = 0.0;
	m_dEndTime = 0.0;
	m_dStepSize = -1.0;
	m_dStepTolerance = CAppSettings().GetStepTolerance();
	m_cData = 0;
	m_cContainer = 0;
	m_bCloseLog = false;
	m_bManager = true;
	m_eMode = SMD_STOP;
	m_bViewer = false;
	m_bTerminated = false;
	m_ucState = 0;
#if 0 // TODO: To add
	m_cStore = 0;
#endif
	m_cSimData = 0;
	m_lTimer = 0;
}

isl::CConnect::CConnect(bool bOwner)
{
	boost::filesystem::path bpLogFile(boost::filesystem::temp_directory_path());
	bpLogFile.append("isl_api.log");
	AppLog->Init(bpLogFile.string());
	m_uType = 0;
	m_ulPID = boost::this_process::get_id();
	m_nConnectTimeOut = 0;
	m_dStartTime = 0.0;
	m_dEndTime = 0.0;
	m_dStepSize = -1.0;
	m_dStepTolerance = CAppSettings().GetStepTolerance();
	m_cData = 0;
	m_cContainer = 0;
	m_bCloseLog = false;
	m_bManager = bOwner;
	m_eMode = SMD_STOP;
	m_bViewer = false;
	m_bTerminated = false;
	m_ucState = 0;
#if 0 // TODO: To add
	m_cStore = 0;
#endif
	m_cSimData = 0;
	m_lTimer = 0;
}

isl::CConnect::~CConnect()
{
	AppLogInfo(ISLCONNECT_UNLOADING_API, "Unloading the ISL API connector");
	if ((m_ucState == 3) || (m_ucState == 7)) {
		Disconnect();
	}
	//
#if 0 // TODO: To add
	CData::CloseComputeLibrary();
#endif
	//
	m_lIns.clear();
	m_lOuts.clear();
	m_mIOs.clear();
	m_mIns.clear();
	m_mOuts.clear();
	if (m_lIOs.empty() == false) {
		std::vector<CData *>::iterator iElt = m_lIOs.begin();
		while (iElt != m_lIOs.end()) {
			delete *iElt;
			iElt = m_lIOs.erase(iElt);
		}
	}
	//
#if 0 // TODO: To add
	CloseStore();
#endif
	if (m_bCloseLog) {
		AppLog_Close;
	}
}

bool isl::CConnect::New(const std::string & sName)
{
	if (sName.empty()) {
		AppLogError(ISLCONNECT_NEW_NAMEREQUIRED, "A model name is required to create a new connector.");
		return false;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_NEW_ALREADYCONNNECTED,
			"Connector '%s' is already connected.", m_sName.c_str());
		return true;
	}
	m_sName = sName;
	// Creating the log file in the working directory
	AppLog->Init(boost::str(boost::format("%1%.log") % m_sName));
	AppLogInfo(99999, "OpenISL Version: " GET_APP_VERSION(VERSION_NUMBER));
	AppLogInfo(99997, "Runtime path: %s", isl::CApplication::GetRuntimePath().c_str());
	AppLogInfo(ISLCONNECT_CREATING_NEWCONFIG,
		"Creating a new connector configuration for model '%s'...", m_sName.c_str());
	//
	m_sId = "{" + GenUID() + "}";
	AppLogInfo(ISLCONNECT_NEWMODEL_UUID, "Connector '%s': UUID set to: %s",
		m_sName.c_str(), m_sId.c_str());
	m_uType = CString::FromStrToInt("ISL");
	AppLogInfo(ISLCONNECT_NEWMODEL_TYPE, "Connector '%s': Type set to: 'ISL'",
		m_sName.c_str());
	//
	m_dStartTime = 0.0;
	m_dEndTime = 10.0;
	m_dStepSize = 1.0;
	AppLogInfo(ISLCONNECT_NEWMODEL_TIMES,
		"Connector '%s': Times definition: From %gs to %gs every %gs.",
		m_sName.c_str(), m_dStartTime, m_dEndTime, m_dStepSize);
	//
	AppLogInfo(ISLCONNECT_NEWMODEL_CREATED, "Connector '%s': a new instance has been created.",
		m_sName.c_str());
	return true;
}

bool isl::CConnect::Load(const std::string & sFileName)
{
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_LOAD_ALREADYCONNNECTED,
			"Connector '%s' is already connected.", m_sName.c_str());
		return true;
	}
	boost::filesystem::path bpFile(sFileName);
	if (boost::filesystem::exists(bpFile) == false) {
		AppLogError(ISLCONNECT_LOAD_FILENOTEXIST,
			"Cannot load the file '%s'.", bpFile.string().c_str());
		return false;
	}
	boost::filesystem::path bpLog(bpFile);
	bpLog.replace_extension("log");
	AppLog->Init(bpLog.string());
	AppLogInfo(99999, "OpenISL Version: " GET_APP_VERSION(VERSION_NUMBER));
	AppLogInfo(99997, "Runtime path: %s", isl::CApplication::GetRuntimePath().c_str());
	CXML xFile(bpFile.string());
	AppLogInfo(ISLCONNECT_LOADING_FILE, "Loading the file '%s'...", bpFile.string().c_str());
	if (xFile.Load() == false) {
		AppLogError(ISLCONNECT_LOAD_LOADFAILED, "File '%s': loading error: %d, %s.",
			xFile.GetFile().c_str(), xFile.GetStatus(), xFile.GetParserError().c_str());
		return false;
	}
	AppLogInfo(ISLCONNECT_PARSING_FILE, "Parsing file '%s'...", bpFile.string().c_str());
	if (xFile.Parse() == false) {
		AppLogError(ISLCONNECT_LOAD_PARSEFAILED, "File '%s': Parsing error: %d, %s.",
			xFile.GetFile().c_str(), xFile.GetStatus(), xFile.GetParserError().c_str());
		return false;
	}
	m_sFileName = bpFile.string();
	// Model information
	if (xFile.GetRoot() == 0) {
		AppLogError(ISLCONNECT_LOAD_IMPORTFAILED,
			"File '%s': Failed to import the configuration (%s).",
			m_sFileName.c_str(), "root");
		return false;
	}
	CXMLNode * xModel = xFile.GetRoot()->GetNode("Model");
	if (xModel == 0) {
		AppLogError(ISLCONNECT_LOAD_IMPORTFAILED,
			"File '%s': Failed to import the configuration (%s).",
			m_sFileName.c_str(), "Model");
		return false;
	}
	CXMLNode * xInfo = xModel->GetNode("Information");
	if (xInfo == 0) {
		AppLogError(ISLCONNECT_LOAD_IMPORTFAILED,
			"File '%s': Failed to import the configuration (%s).",
			m_sFileName.c_str(), "Information");
		return false;
	}
	m_sName = xInfo->GetAttribute("name");
	m_sId = xInfo->GetAttribute("id");
	std::string sType = xInfo->GetAttribute("type");
	m_uType = CString::FromStrToInt(sType);
	// Model co-simulation parameters
	CXMLNode * xCosim = xModel->GetNode("Cosimulation");
	if (xCosim == 0) {
		AppLogError(ISLCONNECT_LOAD_IMPORTFAILED,
			"File '%s': Failed to import the configuration (%s).",
			m_sFileName.c_str(), "Cosimulation");
		return false;
	}
	SetSessionId(xCosim->GetAttribute("session"));
	m_nConnectTimeOut = CString::GetInteger(xCosim->GetAttribute("connecttimeout"));
	m_dStartTime = CString::GetDouble(xCosim->GetAttribute("starttime"));
	m_dEndTime = CString::GetDouble(xCosim->GetAttribute("endtime"));
	m_dStepSize = CString::GetDouble(xCosim->GetAttribute("stepsize"));
	if (m_dStepSize == 0.0) {
		m_dStepSize = 1.0; // No step size = event driven mode
	}
	m_dStepTolerance = CString::GetDouble(xCosim->GetAttribute("steptolerance"));
	if (m_dStepTolerance == 0.0) {
		m_dStepTolerance = CAppSettings().GetStepTolerance();
	}
	// Load new data types definition
	// TODO: Add management of new types defintion
	// Get the variables
	CXMLNode * xVars = xModel->GetNode("Variables");
	if (xVars == 0) {
		AppLogError(ISLCONNECT_LOAD_FAILED_TOGETVARS,
			"File '%s': Failed to extarct variables.", xFile.GetFile().c_str());
		return false;
	}
	int nVars = xVars->CountNodes();
	if (nVars <= 0) {
		AppLogError(ISLCONNECT_LOAD_NOVARS,
			"File '%s': No variables have been loaded.", xFile.GetFile().c_str());
		return false;
	}
	// Build the data structure
	for (int i = 0; i < nVars; i++) {
		CXMLNode * xVar = xVars->GetNode(i);
		if (xVar) {
			CData * cVar = new CData(this, xVar->GetAttribute("id"));
			cVar->SetName(xVar->GetAttribute("name"));
			cVar->SetConnectId(xVar->GetAttribute("connectid"));
			cVar->SetCausality(xVar->GetAttribute("causality"));
			double dVal = CString::GetDouble(xVar->GetAttribute("stepsize"));
			if (dVal == 0.0) {
				dVal = -1.0;
			}
			cVar->SetStepSize(dVal);
			if (xVar->GetAttribute("store") == "true") {
				cVar->UseStore(true);
			}
			int nVal = CString::GetInteger(xVar->GetAttribute("synctimeout"));
			if (nVal == 0) {
				nVal = -1; // Check if we keep the value of 0 or not
			}
			cVar->SetSyncTimeout(nVal);
			// Data types
			std::string sInit;
			int nSize = 0;
			CDataType::tType eType = CDataType::TP_UNKNOWN;
			if (xVar->GetNode("Real")) {
				nSize = CString::GetInteger(xVar->GetNode("Real")->GetAttribute("size"));
				sInit = xVar->GetNode("Real")->GetAttribute("initialvalue");
				eType = CDataType::TP_REAL;
			}
			else if (xVar->GetNode("Integer")) {
				nSize = CString::GetInteger(xVar->GetNode("Integer")->GetAttribute("size"));
				sInit = xVar->GetNode("Integer")->GetAttribute("initialvalue");
				eType = CDataType::TP_INTEGER;
			}
			else if (xVar->GetNode("Boolean")) {
				nSize = CString::GetInteger(xVar->GetNode("Boolean")->GetAttribute("size"));
				sInit = xVar->GetNode("Boolean")->GetAttribute("initialvalue");
				eType = CDataType::TP_INTEGER;
			}
			else if (xVar->GetNode("String")) {
				nSize = CString::GetInteger(xVar->GetNode("String")->GetAttribute("size"));
				sInit = xVar->GetNode("String")->GetAttribute("initialvalue");
				eType = CDataType::TP_INTEGER;
			}
			if (eType != CDataType::TP_UNKNOWN) {
				if (nSize == 0) {
					nSize = 1;
				}
				if (cVar->SetType(eType, nSize)) {
					// Allocate the memory
					cVar->GetType()->Allocate();
					// Set the initial value
					cVar->GetType()->SetInitial(sInit);
				}
			}
			// TODO: Add the compute elements
			// Only fill the main vector...
			// ... additional vectors and maps will be filled during the check
			m_lIOs.push_back(cVar);
		}
	}
	AppLogInfo(ISLCONNECT_LOAD_COMPLETED, "Connector '%s': File '%s' loaded.",
		m_sName.c_str(), m_sFileName.c_str());
	return true;
}

bool isl::CConnect::Check()
{
	if ((m_ucState == 3) || (m_ucState == 7)) {
		AppLogWarning(ISLCONNECT_CHECK_ALREADYCONNNECTED,
			"Connector '%s' is already connected and cannot be checked.", m_sName.c_str());
		return true;
	}
	if (m_ucState == 1) {
		AppLogWarning(ISLCONNECT_CHECK_ALREADYCHECKED,
			"Connector '%s' is already connected and cannot be checked.", m_sName.c_str());
		return true;
	}
	AppLogInfo(ISLCONNECT_CHECK_RUNNING,
		"Connector '%s': checking the defintion of connector...", m_sName.c_str());
	m_ucState = 0;
	// Model information
	if (m_sName.empty()) {
		AppLogError(ISLCONNECT_CHECK_NONAME, "Unknown name of the connector.");
		return false;
	}
	if (m_sId.empty()) {
		AppLogWarning(ISLCONNECT_CHECK_NOMODELID,
			"Connector '%s': no model ID. A new one will be set.", m_sName.c_str());
		m_sId = "{" + GenUID() + "}";
	}
	if (m_uType == 0) {
		AppLogWarning(ISLCONNECT_CHECK_NOTYPE,
			"Connector '%s': no model type. The type will be set to 'ISL'.", m_sName.c_str());
		m_uType = CString::FromStrToInt("ISL");
	}
	AppLogInfo(ISLCONNECT_CHECK_MODELINFO,
		"Connector '%s': Model information:\n\tFile name: %s\n\tName: %s\n\tId: %s\n\tType: %s\n\tPID: %u",
		m_sName.c_str(), m_sFileName.c_str(), m_sName.c_str(), m_sId.c_str(),
		CString::FromIntToStr(m_uType).c_str(), m_ulPID);
	// Co-simulation parameters
	if (m_sSessionId.empty()) {
		AppLogWarning(ISLCONNECT_CHECK_NOSESSIONID, "Connector '%s': no session identifier is defined."
			" The session id shall be set during the 'Create' function call.");
	}
	if (m_nConnectTimeOut < 0) {
		AppLogWarning(ISLCONNECT_CHECK_NEGCONNECTTIMEOUT,
			"Connector '%s': Connection time out has been set to 0. Negative numbers are not used.",
			m_sName.c_str());
		m_nConnectTimeOut = 0;
	}
	if (m_dStepSize == -1.0) {
		AppLogInfo(ISLCONNECT_CHECK_EVENTMODE,
			"Connector '%s': Event driven mode enabled (step size == -1).", m_sName.c_str());
	}
	else {
		// Time based mode... checking times
		if (m_dStartTime < 0.0) {
			AppLogError(ISLCONNECT_CHECK_NEGSTARTTIME, "Connector '%s': Start time (%gs) cannot be negative.",
				m_sName.c_str(), m_dStartTime);
			return false;
		}
		if (m_dEndTime < 0.0) {
			AppLogError(ISLCONNECT_CHECK_NEGENDTIME, "Connector '%s': End time (%gs) cannot be negative.",
				m_sName.c_str(), m_dEndTime);
			return false;
		}
		if (m_dStepSize < 0.0) {
			AppLogError(ISLCONNECT_CHECK_NEGSTEPSIZE, "Connector '%s': Step size (%gs) cannot be negative.",
				m_sName.c_str(), m_dStepSize);
			return false;
		}
		if (m_dStartTime > 0.0) {
			AppLogWarning(ISLCONNECT_CHECK_NONZEROSTARTTIME,
				"Connector '%s': Start time is set to a time different from 0.", m_sName.c_str());
		}
		if (m_dStartTime >= m_dEndTime) {
			AppLogError(ISLCONNECT_CHECK_ENDBEFORESTART,
				"Connector '%s': End time (%gs) <= Start Time (%gs).", m_sName.c_str(), m_dEndTime, m_dStartTime);
			return false;
		}
		if (m_dStepSize >= m_dEndTime - m_dStartTime) {
			AppLogError(ISLCONNECT_CHECK_WRONGSTEPSIZE,
				"Connector '%s': The step size (%gs) cannot be greater than the difference between the end (%gs) and start (%gs) times.",
				m_sName.c_str(), m_dStepSize, m_dEndTime, m_dStartTime);
			return false;
		}
	}
	if ((m_dStepTolerance <= 0.0) || (m_dStepTolerance >= 1.0)) {
		AppLogWarning(ISLCONNECT_CHECK_WRONGSTEPTOLERANCE,
			"Connector '%s': Wrong step tolerance (%g). The default value (%g) will be used.",
			m_sName.c_str(), m_dStepTolerance, DEFAULT_STEP_TOLERANCE);
		m_dStepTolerance = DEFAULT_STEP_TOLERANCE;
	}
	AppLogInfo(ISLCONNECT_CHECK_COSIMINFO,
		"Connector '%s': Co-simulation parameters:\n\tSession id: %s\n\tStart time: %gs\n\tEnd time: %gs\n"
		"\tStep size: %gs\n\tStep tolerance: %g\n\tConnection timeout: %ds",
		m_sName.c_str(), m_sSessionId.c_str(), m_dStartTime, m_dEndTime, m_dStepSize, m_dStepTolerance, m_nConnectTimeOut);
	// Check the variables, refine the parameters and build the maps
	if (m_lIOs.empty()) {
		AppLogError(ISLCONNECT_CHECK_NOIOS, "Connector '%s': no variables have been defined.", m_sName.c_str());
		return false;
	}
	bool bIssueFound = false;
	for (size_t i = 0; i < m_lIOs.size(); i++) {
		CData * cIO = m_lIOs[i];
		if (cIO->IsChecked()) {
			continue;
		}
		if (cIO->Check() == false) {
			AppLogError(ISLCONNECT_CHECK_IOFAILED, "Connector '%s': Variable '%s': Check failed.",
				m_sName.c_str(), cIO->GetId().c_str());
			bIssueFound = true;
		}
		if (cIO->IsOutput() && (m_bManager == false)) {
			// If m_bManager is false (viewer mode) then all outputs becomes inputs
			cIO->SetCausality(CData::CS_INPUT);
		}
		if (m_mIOs.count(cIO->GetId()) == 0) {
			m_mIOs[cIO->GetId()] = cIO;
			if (cIO->IsInput()) {
				m_lIns.push_back(cIO);
				m_mIns[cIO->GetId()] = cIO;
			}
			else if (cIO->IsOutput()) {
				m_lOuts.push_back(cIO);
				m_mOuts[cIO->GetId()] = cIO;
			}
		}
	}
	if (bIssueFound) {
		return false;
	}
	//
	AppLogInfo(ISLCONNECT_CHECK_COMPLETED, "Connector '%s': check completed.", m_sName.c_str());
	m_ucState = 1;
	return true;
}

bool isl::CConnect::Save(const std::string & sFileName)
{
	if (m_ucState == 0) {
		AppLogWarning(ISLCONNECT_SAVE_NOTCHECKED,
			"Connector '%s': Before saving the configuration, it will be checked first.",
			m_sName.c_str());
		if (Check() == false) {
			AppLogError(ISLCONNECT_SAVE_CHECKFAILED,
				"Connector '%s': The configuration file cannot be saved.", m_sName.c_str());
			return false;
		}
	}
	if (sFileName.empty() == false) {
		m_sFileName = sFileName;
	}
	if (m_sFileName.empty()) {
		m_sFileName = m_sName + ".xml";
		AppLogWarning(ISLCONNECT_SAVE_NOFILENAME,
			"Connector '%s': the file name cannot be an empty string. The filename is set to '%s'",
			m_sName.c_str(), m_sFileName.c_str());
	}
	AppLogInfo(ISLCONNECT_SAVE_SAVING, "Connector '%s': Saving file '%s'...",
		m_sName.c_str(), m_sFileName.c_str());
	CXML xFile(m_sFileName);
	CXMLNode * xModel = xFile.GetRoot()->AddNode("Model");
	// Model information
	CXMLNode * xInfo = xModel->AddNode("Information");
	xInfo->SetAttribute("name", m_sName);
	xInfo->SetAttribute("id", m_sId);
	xInfo->SetAttribute("type", GetTypeAsStr());
	// Co-simulation parameters
	CXMLNode * xCosim = xModel->AddNode("Cosimulation");
	if (m_sSessionId.empty() == false) {
		xCosim->SetAttribute("session", m_sSessionId);
	}
	xCosim->SetAttribute("connecttimeout", boost::str(boost::format("%1%") % m_nConnectTimeOut));
	xCosim->SetAttribute("starttime", boost::str(boost::format("%1%") % m_dStartTime));
	xCosim->SetAttribute("endtime", boost::str(boost::format("%1%") % m_dEndTime));
	xCosim->SetAttribute("stepsize", boost::str(boost::format("%1%") % m_dStepSize));
	xCosim->SetAttribute("steptolerance", boost::str(boost::format("%1%") % m_dStepTolerance));
	// Types definition
	// TODO: To implement
	// Variables
	if (m_lIOs.empty() == false) {
		CXMLNode * xVars = xModel->AddNode("Variables");
		for (size_t i = 0; i < m_lIOs.size(); i++) {
			CXMLNode * xVar = xVars->AddNode("Variable");
			xVar->SetAttribute("id", m_lIOs[i]->GetId());
			xVar->SetAttribute("name", m_lIOs[i]->GetName());
			xVar->SetAttribute("connectid", m_lIOs[i]->GetConnectId());
			xVar->SetAttribute("causality", m_lIOs[i]->GetCausalityAsStr());
			if (m_dStepSize != m_lIOs[i]->GetOriginalStep()) {
				xVar->SetAttribute("stepsize", boost::str(boost::format("%1%") % m_lIOs[i]->GetOriginalStep()));
			}
			xVar->SetAttribute("store", (m_lIOs[i]->IsStoreUsed() ? "true" : "false"));
			xVar->SetAttribute("synctimeout", boost::str(boost::format("%1%") % m_lIOs[i]->GetSyncTimeout()));
			CXMLNode * xType = xVar->AddNode(m_lIOs[i]->GetType()->GetIdAsStr());
			xType->SetAttribute("size", boost::str(boost::format("%1%") % m_lIOs[i]->GetType()->GetSize()));
			xType->SetAttribute("initialvalue", m_lIOs[i]->GetType()->GetInitialAsStr());
			// Compute (extrapolation)
			// TODO: To implement
		}
	}
	// Saving
	if (xFile.Save() == false) {
		AppLogError(ISLCONNECT_SAVE_FAILED, "Connector '%s': Failed to save the file '%s'.",
			m_sName.c_str(), m_sFileName.c_str());
		return false;
	}
	AppLogInfo(ISLCONNECT_SAVE_SAVED, "Connector '%s': file '%s' saved.",
		m_sName.c_str(), m_sFileName.c_str());
	return true;
}

void isl::CConnect::SetName(const std::string & sName)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_sName = sName;
}

std::string isl::CConnect::GetName()
{
	return m_sName;
}

std::string isl::CConnect::GetId()
{
	if (m_sId.empty() == false) {
		return m_sId;
	}
	m_sId = "{" + GenUID() + "}";
	AppLogInfo(ISLCONNECT_GETID_NEWUUID, "Connector '%s': UUID set to: %s",
		m_sName.c_str(), m_sId.c_str());
	return m_sId;
}

unsigned int isl::CConnect::GetUId()
{
	return std::hash<std::string>()(m_sId);
}

void isl::CConnect::SetType(const std::string & sType)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_uType = CString::FromStrToInt(sType);
}

unsigned int isl::CConnect::GetType()
{
	return m_uType;
}

std::string isl::CConnect::GetTypeAsStr()
{
	return CString::FromIntToStr(m_uType);
}

void isl::CConnect::SetPID(unsigned long ulPId)
{
	m_ulPID = ulPId;
}

unsigned long isl::CConnect::GetPID()
{
	return m_ulPID;
}

std::string isl::CConnect::GetFileName()
{
	return m_sFileName;
}

bool isl::CConnect::SetSessionId(const std::string & sSessionId)
{
	if ((m_ucState != 0) && (m_ucState != 1)) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.",
			m_sName.c_str());
		return false;
	}
	// Check the session name : shall start with a letter or underscore
	if (isl::CString::IsIdentifier(sSessionId) != 0) {
			AppLogError(ISLCONNECT_SETSESSION_WRONGFORMAT,
			"Connector '%s': A session shall start with a letter and shall contain only letters,"
			" digits or underscore characters.", m_sName.c_str());
		return false;
	}
	m_sSessionId = sSessionId;
	return true;
}

std::string isl::CConnect::GetSessionId()
{
	return m_sSessionId;
}

void isl::CConnect::SetConnectTimeOut(int nTimeOut)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_nConnectTimeOut = nTimeOut;
}

int isl::CConnect::GetConnectTimeOut()
{
	return m_nConnectTimeOut;
}

void isl::CConnect::SetStartTime(double dVal)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_dStartTime = dVal;
}

double isl::CConnect::GetStartTime()
{
	return m_dStartTime;
}

void isl::CConnect::SetEndTime(double dVal)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_dEndTime = dVal;
}

double isl::CConnect::GetEndTime()
{
	return m_dEndTime;
}

void isl::CConnect::SetStepSize(double dVal)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_dStepSize = dVal;
}

double isl::CConnect::GetStepSize()
{
	return m_dStepSize;
}

void isl::CConnect::SetStepTolerance(double dVal)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return;
	}
	m_dStepTolerance = dVal;
}

double isl::CConnect::GetStepTolerance()
{
	return m_dStepTolerance;
}

isl::CData * isl::CConnect::NewIO(const std::string & sId, CData::tCausality eCausality, CDataType::tType eType, int nSize)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return 0;
	}
	if (sId.empty()) {
		AppLogError(ISLCONNECT_NEWIO_NOIDNONAME,
			"Connecotr '%s': The new variable has no identifier.", m_sName.c_str());
		return 0;
	}
	if (IOExists(sId)) {
		AppLogError(ISLCONNECT_NEWIO_IOALREADYEXIST,
			"Connector '%s': Variable '%s': The variable already exists.", m_sName.c_str(), sId.c_str());
		return 0;
	}
	CData * cIO = new CData(this, sId);
	cIO->SetType(eType, nSize);
	cIO->SetCausality(eCausality);
	m_lIOs.push_back(cIO);
	AppLogInfo(ISLCONNECT_NEWIO_ADDED, "Connector '%s': Variable '%s' added.",
		m_sName.c_str(), cIO->GetId().c_str());
	return cIO;
}

bool isl::CConnect::AddIO(CData * cData)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return false;
	}
	if (cData == 0) {
		AppLogError(ISLCONNNECT_ADDIO_NODATA, "Connector '%s': No variable to remove.", m_sName.c_str());
		return false;
	}
	if (IOExists(cData->GetId())) {
		AppLogError(ISLCONNECT_ADDIO_IOALREADYEXIST,
			"Connector '%s': Variable '%s': The variable already exists.",
			m_sName.c_str(), cData->GetId().c_str());
		return false;
	}
	m_lIOs.push_back(cData);
	AppLogInfo(ISLCONNECT_ADDIO_ADDED, "Connector '%s': Variable '%s' added.",
		m_sName.c_str(), cData->GetId().c_str());
	return true;
}

bool isl::CConnect::RemoveIO(CData * cData)
{
	if (m_ucState == 1) {
		m_ucState = 0;
	}
	if (m_ucState != 0) {
		AppLogWarning(ISLCONNECT_CANNOTMODIFY_AFTERCONNECT,
			"Connector '%s': Cannot modify the connector configuration after connection.", m_sName.c_str());
		return false;
	}
	if (cData == 0) {
		AppLogError(ISLCONNNECT_REMOVEIO_NODATA, "Connector '%s': No variable to remove.", m_sName.c_str());
		return false;
	}
	CData::tCausality eCausality = cData->GetCausality();
	bool bFound = false;
	if (m_lIOs.empty() == false) {
		std::vector<CData *>::iterator iIO;
		for (iIO = m_lIOs.begin(); iIO != m_lIOs.end(); ++iIO) {
			if (*iIO == cData) {
				m_lIOs.erase(iIO);
				bFound = true;
				break;
			}
		}
		if (eCausality == CData::CS_INPUT) {
			for (iIO = m_lIns.begin(); iIO != m_lIns.end(); ++iIO) {
				if (*iIO == cData) {
					m_lIns.erase(iIO);
					break;
				}
			}
		}
		else if (eCausality == CData::CS_OUTPUT) {
			for (iIO = m_lOuts.begin(); iIO != m_lOuts.end(); ++iIO) {
				if (*iIO == cData) {
					m_lOuts.erase(iIO);
					break;
				}
			}
		}
	}
	if (bFound) {
		m_mIOs.erase(cData->GetId());
		m_mIns.erase(cData->GetId());
		m_mOuts.erase(cData->GetId());
		AppLogInfo(ISLCONNECT_REMOVEIO_REMOVED, "Connector '%s': Variable '%s' removed.",
			m_sName.c_str(), cData->GetId().c_str());
		delete cData;
		return true;
	}
	AppLogWarning(ISLCONNECT_REMOVEIO_NOTFOUND, "Connector '%s': Variable '%s' not found.",
		m_sName.c_str(), cData->GetId().c_str());
	return false;
}

int isl::CConnect::GetNbIOs()
{
	return m_lIOs.size();
}

int isl::CConnect::GetNbInputs()
{
	return m_lIns.size();
}

int isl::CConnect::GetNbOutputs()
{
	return m_lOuts.size();
}

bool isl::CConnect::IOExists(const std::string & sId)
{
	if (m_lIOs.empty()) {
		return false;
	}
	return (GetIO(sId) != 0);
}

isl::CData * isl::CConnect::GetIO(const std::string & sId, int * nInd)
{
	if (m_sId.empty()) {
		AppLogError(ISLCONNECT_GETIO_NOID, "Connector '%s': The id is missing to identify the variable.",
			m_sName.c_str());
		return 0;
	}
	if (m_lIOs.empty()) {
		AppLogError(ISLCONNECT_GETIO_NOIOS,
			"Connector '%s': No variable have been defined.", m_sName.c_str());
		return 0;
	}
	try {
		if ((m_ucState != 0) && (nInd == 0)) {
			return m_mIOs.at(sId);
		}
		else {
			for (size_t i = 0; i < m_lIOs.size(); i++) {
				if (m_lIOs[i]->GetId() == sId) {
					if (nInd != 0) {
						*nInd = i;
					}
					return m_lIOs[i];
				}
			}
		}
	}
	catch (...) {
		return 0;
	}
	return 0;
}

isl::CData * isl::CConnect::GetIO(int i)
{
	if (m_sId.empty()) {
		AppLogError(ISLCONNECT_GETIO_NOID, "Connector '%s': The id is missing to identify the variable.",
			m_sName.c_str());
		return 0;
	}
	if (m_lIOs.empty()) {
		AppLogError(ISLCONNECT_GETIO_NOIOS,
			"Connector '%s': No variable have been defined.", m_sName.c_str());
		return 0;
	}
	try {
		return m_lIOs[i];
	}
	catch (...) {
		return 0;
	}
	return 0;
}

isl::CData * isl::CConnect::GetInput(const std::string & sId)
{
	if (m_sId.empty()) {
		AppLogError(ISLCONNECT_GETIN_NOID, "Connector '%s': The id is missing to identify the input.",
			m_sName.c_str());
		return 0;
	}
	if (m_ucState == 0) {
		AppLogWarning(ISLCONNECT_GETIN_NOTCHECKED,
			"Conenctor '%s': List of inputs are built after the 'check' function call.", m_sName.c_str());
		return GetIO(sId);
	}
	try {
		return m_mIns[sId];
	}
	catch (...) {
		return 0;
	}
	return 0;
}

isl::CData * isl::CConnect::GetInput(int i)
{
	try {
		return m_lIns[i];
	}
	catch (...) {
		return 0;
	}
	return 0;
}

isl::CData * isl::CConnect::GetOutput(const std::string & sId)
{
	if (m_sId.empty()) {
		AppLogError(ISLCONNECT_GETOUT_NOID, "Connector '%s': The id is missing to identify the output.",
			m_sName.c_str());
		return 0;
	}
	if (m_ucState == 0) {
		AppLogWarning(ISLCONNECT_GETOUT_NOTCHECKED,
			"Conenctor '%s': List of outputs are built after the 'check' function call.", m_sName.c_str());
		return GetIO(sId);
	}
	try {
		return m_mOuts[sId];
	}
	catch (...) {
		return 0;
	}
	return 0;
}

isl::CData * isl::CConnect::GetOutput(int i)
{
	try {
		return m_lOuts[i];
	}
	catch (...) {
		return 0;
	}
	return 0;
}

void isl::CConnect::CloseLogOnDelete(bool bVal)
{
	m_bCloseLog = bVal;
}

void isl::CConnect::SetMode(tStopMode eMode)
{
	m_eMode = eMode;
}

isl::CConnect::tStopMode isl::CConnect::GetMode()
{
	return m_eMode;
}

void isl::CConnect::SetViewer(bool bVal)
{
	if ((m_ucState == 3) || (m_ucState == 7)) {
		AppLogWarning(ISLCONNECT_VIEWER_BEFORECONNNECT,
			"Connector '%s': The viewer mode can only be modified before establishing the connection.",
			m_sName.c_str());
	}
	m_bViewer = bVal;
}

bool isl::CConnect::IsViewer()
{
	return m_bViewer;
}

void isl::CConnect::SetTerminated(bool bVal)
{
	m_bTerminated = bVal;
}

bool isl::CConnect::IsTerminated()
{
	return m_bTerminated;
}

bool isl::CConnect::IsConnnected()
{
	return m_ucState == 7;
}

bool isl::CConnect::Create(const std::string & sSessionId)
{
	if (m_ucState == 0) {
		AppLogWarning(ISLCONNECT_CREATE_NOTCHECKED,
			"Connector '%s': the configuration must be checked first.", m_sName.c_str());
		if (Check() == false) {
			AppLogError(ISLCONNECT_CREATE_CHECKFAILED,
				"Connector '%s': Cannot create the session after a failed check.", m_sName.c_str());
			return false;
		}
	}
	else if (m_ucState != 1) {
		AppLogError(ISLCONNECT_CREATE_ALREADYRUNNING,
			"Connector '%s': is already connected and running. State: %d",
			m_sName.c_str(), m_ucState);
		return false;
	}
	// Check the session name
	if (sSessionId.empty() == false) {
		if (SetSessionId(sSessionId) == false) {
			AppLogError(ISLCONNECT_CREATE_NOVALIDSESSION,
				"Connector '%s': A valid session id is required to create the session.",
				m_sName.c_str());
			return false;
		}
	}
	if (m_sSessionId.empty()) {
		AppLogError(ISLCONNECT_CREATE_NOVALIDSESSION,
			"Connector '%s': no valid session id is defined.", m_sName.c_str());
		return false;
	}
	// If in viewer mode, skip the session creation
	if (m_bViewer) {
		AppLogInfo(ISLCONNECT_CREATE_VIEWERMODE,
			"Connector '%s': viewer mode, no session will be created.", m_sName.c_str());
		m_ucState = 3;
		return true;
	}
	// Check if the SHM already exists
	if (m_cContainer != 0) {
		AppLogError(ISLCONNECT_CREATE_SHMEXIST,
			"Connector '%s': is already connected and running. State: %d",
			m_sName.c_str(), m_ucState);
		return false; // Already connected
	}
	// Create the model shared memory
	std::string sSHM(boost::str(boost::format(SHM_MODEL_KEY_ID) % m_sSessionId % GetUId()));
	bool bIsGlobalIPC = CAppSettings().IsGlobalIPC();
	if (bIsGlobalIPC) {
		AppLogInfo(ISLCONNECT_CREATE_ISGLOBALIPC,
			"Connector '%s': Global IPC mode enabled.", m_sName.c_str());
	}
	CSHM * cMem = 0;
	if (CAppSettings().IsISLCompatible()) {
		AppLogInfo(ISLCONNECT_CREATE_ISLCOMPATIBLE,
			"Connector '%s': IPC identifiers are ISL compatible.", m_sName.c_str());
		cMem = new CSHM(sSHM, "qipc_sharedmemory_", bIsGlobalIPC);
	}
	else {
		cMem = new CSHM(sSHM, bIsGlobalIPC);
	}
	if (m_bManager) {
		AppLogInfo(ISLCONNECT_CREATE_CREATESHM, "Connector '%s': creating the shared memory '%s'...",
			m_sName.c_str(), sSHM.c_str());
		// Compute the size
		int nSize = CSHMConnect::GetSizeOf(this);
		if (nSize <= 0) {
			AppLogError(ISLCONNECT_CREATE_WRONGSHMSIZE, "Connector '%s': Wrong shared memory size: %d.",
				m_sName.c_str(), nSize);
			delete cMem;
			return false;
		}
		// Create the SHM
		if (cMem->Create(nSize, CSHM::READWRITE) == false) {
			AppLogError(ISLCONNECT_CREATE_CREATESHMFAILED,
				"Connector '%s': Failed to create the shared memory: %d-%d",
				m_sName.c_str(), cMem->GetStatus(), cMem->GetError());
			delete cMem;
			return false;
		}
	}
	else {
		AppLogInfo(ISLCONNECT_CREATE_CONNECTSHM, "Connector '%s': connecting to the shared memory '%s'...",
			m_sName.c_str(), sSHM.c_str());
		// Connect to the SHM
		if (cMem->Attach(CSHM::READWRITE) == false) {
			AppLogError(ISLCONNECT_CREATE_CONNECTSHMFAILED,
				"Connector '%s': Failed to connect to the shared memory: %d-%d",
				m_sName.c_str(), cMem->GetStatus(), cMem->GetError());
			delete cMem;
			return false;
		}
	}
	m_cContainer = cMem;
	if (cMem->Data() == 0) {
		AppLogError(ISLCONNECT_CREATE_NOVALIDSHMSPACE,
			"Connector '%s': Not a valid memory allocated space.", m_sName.c_str());
		return false;
	}
	CSHMConnect * cData = new CSHMConnect(cMem->Data(), this);
	if (m_ulPID <= 0) {
		m_ulPID = boost::this_process::get_id();
	}
	if (m_bManager) {
		cMem->Lock();
		if (cData->Initialize() == false) {
			AppLogError(ISLCONNECT_CREATE_INITSHMFAILED,
				"Connector '%s': Failed to initialize the shared memory.", m_sName.c_str());
			cMem->Unlock();
			return false;
		}
		cMem->Unlock();
	}
	m_cData = cData;
	// Create the outputs
	if (m_lOuts.empty() == false) {
		bool bOk = true;
		for (size_t i = 0; i < m_lOuts.size(); i++) {
			// Do not connect data without SignalID
			if (m_lOuts[i]->GetConnectId().empty()) {
				continue;
			}
			int nRet = m_lOuts[i]->Connect(false);
			if (nRet != 0) {
				AppLogError(ISLCONNECT_CREATE_DATACONNECTFAILED,
					"Connector '%s': Variable '%s': failed to connect the output: %d.",
					m_sName.c_str(), m_lOuts[i]->GetId().c_str(), nRet);
				bOk = false;
			}
			// Initialize the values in the shared memory
			m_lOuts[i]->Initialize(m_dStartTime);
		}
		if (bOk == false) {
			return false;
		}
	}
	if (m_bManager) {
		// ISL shared memory
		ISLSims->SetId(GetUId());
		ISLSims->SetPID(m_ulPID);
		ISLSims->SetName(GetName());
		ISLSims->SetSessionId(m_sSessionId);
		AppLogInfo(ISLCONNECT_CREATE_ISLSIMADD,
			"Connector '%s': Adding the connector to the simulations management utility.",
			m_sName.c_str());
		m_cSimData = ISLSims->Add();
		if (m_cSimData == 0) {
			AppLogError(ISLCONNECT_CREATE_ISLSIMSFAILED,
				"Connector '%s': Error from the simulations management utility.", m_sName.c_str());
			return false;
		}
	}
	//
	AppLogInfo(ISLCONNECT_CREATE_CREATED,
		"Connector '%s': the session is created and the outputs connected.", m_sName.c_str());
	m_bTerminated = false;
	m_ucState = 3;
	return true;
}

bool isl::CConnect::Connect(bool bWait)
{
	if (m_ucState != 3) {
		AppLogError(ISLCONNECT_CONNECT_NOTREADY,
			"Connector '%s': cannot be connected to the transmitters. State: %d",
			m_sName.c_str(), m_ucState);
		return false;
	}
	// If viewer mode, call ConnectAsViewer
	if (m_bViewer) {
		return ConnectAsViewer(bWait);
	}
	// Add the connector instance in the instances management utility
	ISLInstances->Add(this);
	// Connect the inputs
	if (m_lIns.empty() == false) {
		bool bOk = true;
		for (size_t i = 0; i != m_lIns.size(); i++) {
			// Do not connect data without ConnectID
			if (m_lIns[i]->GetConnectId().empty()) {
				continue;
			}
			if (m_lIns[i]->Connect(bWait, m_nConnectTimeOut) < 0) {
				AppLogError(ISLCONNECT_CONNECT_FAILEDINPUT,
					"Connector '%s': Variable '%s': failed to connect the input to the transmitters.",
					m_sName.c_str(), m_lIns[i]->GetId().c_str());
				bOk = false;
			}
		}
		if (bOk == false) {
			return false;
		}
	}
	// Start timer if set
	StartTimer();
	//
	AppLogInfo(ISLCONNECT_CONNECT_CONNECTED,
		"Connector '%s': inputs connected to the transmitters.", m_sName.c_str());
	m_ucState = 7;
	return true;
}

bool isl::CConnect::Disconnect()
{
	if ((m_ucState !=  3) && (m_ucState != 7)) {
		AppLogError(ISLCONNECT_DISCONNECT_NOTCONNECTED,
			"Connector '%s': not yet connected to a session.", m_sName.c_str());
		return false;
	}
	// If viewer mode, call the method DisconnectAsViewer
	if (m_bViewer) {
		return DisconnectAsViewer();
	}
	// Compute and display the elapsed time
	if (m_lTimer != 0) {
		StopTimer();
		AppLogInfo(ISLCONNECT_DISCONNECT_TIMER,
			"Connector '%s': elapsed time: %lds", m_sName.c_str(), m_lTimer);
		m_lTimer = 0;
	}
	// ISL simulations management tool
	if (m_cSimData != 0) {
		AppLogInfo(ISLCONNECT_DISCONNECT_ISLSIMSREMOVE,
			"Connector '%s': Removing the connection from the simulation management utility.",
			m_sName.c_str());
		ISLSims->Remove(m_cSimData);
	}
	m_cSimData = 0;
	ISLSims_Close;
	// Detach the shared memory and clean
	bool bRet = true;
	if (m_cContainer != 0) {
		// If it returns false, it means that the shared memory is still used by another process
		AppLogInfo(ISLCONNECT_DISCONNECT_DETACHSHM,
			"Connector '%s': detach shared memory...", m_sName.c_str());
		bRet = m_cContainer->Detach();
		delete m_cContainer;
	}
	m_cContainer = 0;
	if (m_cData != 0) {
		delete m_cData;
	}
	m_cData = 0;
	// Disconnect data
	for (size_t i = 0; i < m_lIOs.size(); i++) {
		// Do not disconnect data without ConnectID
		if (m_lIOs[i]->GetConnectId().empty()) {
			continue;
		}
		if (m_lIOs[i]->Disconnect() == false) {
			AppLogError(ISLCONNECT_DISCONNECT_FAILEDIO,
				"Connector '%s': Variable '%s': Failed to disconnect.",
				m_sName.c_str(), m_lIOs[i]->GetId().c_str());
			bRet = false;
		}
	}
	AppLogInfo(ISLCONNECT_DISCONNECT_DISCONNECTED,
		"Connector '%s': disconnected from the session.", m_sName.c_str());
	m_bTerminated = true;
	m_ucState = 1; // Return in 'checked' mode
	return bRet;
}

bool isl::CConnect::ConnectAsViewer(bool bWait)
{
	// State already checked in the Connect method
	// The connector is not managed in the ISLInstances utility
	// Connect all IOs as inputs
	if (m_lIOs.empty() == false) {
		bool bOk = true;
		for (size_t i = 0; i < m_lIOs.size(); i++) {
			// Do not connect data without ConnectID
			if (m_lIOs[i]->GetConnectId().empty()) {
				continue;
			}
			if (m_lIOs[i]->ConnectAsViewer(bWait) == false) {
				AppLogError(ISLCONNECT_CONNECTVIEWER_FAILEDIO,
					"Connector '%s': Variable '%s': failed to connect the variable to the transmitters.",
					m_sName.c_str(), m_lIOs[i]->GetId().c_str());
				bOk = false;
			}
		}
		if (bOk == false) {
			return false;
		}
	}
	//
	AppLogInfo(ISLCONNECT_CONNECTVIEWER_CONNECTED,
		"Connector '%s': IOs connected to the transmitters.", m_sName.c_str());
	m_ucState = 7;
	return true;
}

bool isl::CConnect::DisconnectAsViewer()
{
	// State already checked in the Connect method
	// Disconnect all connected IOs
	bool bRet = true;
	if (m_lIOs.empty()) {
		for (size_t i = 0; i < m_lIOs.size(); i++) {
			// Do not disconnect IOs without ConnectID
			if (m_lIOs[i]->GetConnectId().empty()) {
				continue;
			}
			if (m_lIOs[i]->DisconnectAsViewer() == false) {
				AppLogError(ISLCONNECT_DISCONNECTVIEWER_FAILEDIO,
					"Connector '%s': Variable '%s': Failed to disconnect.",
					m_sName.c_str(), m_lIOs[i]->GetId().c_str());
				bRet = false;
			}
		}
	}
	AppLogInfo(ISLCONNECT_DISCONNECTVIEWER_DISCONNECTED,
		"Connector '%s': disconnected from the session.", m_sName.c_str());
	m_bTerminated = true;
	m_ucState = 1; // Return in 'checked' mode
	return bRet;
}

void isl::CConnect::StartTimer()
{
	m_lTimer = time(0);
}

void isl::CConnect::StopTimer()
{
	m_lTimer = time(0) - m_lTimer;
}

bool isl::CConnect::ListenToExitSession()
{
	CExitThread * cThread = new CExitThread(this, true);
	cThread->Start();
	return true;
}

bool isl::CConnect::SendStopSession()
{
	return CExitThread::SendStopRequest(m_sSessionId);
}

#if 0 // TODO: To implement
bool isl::CConnect::OpenStore()
{
	// Close the database if it as been already open
	CloseStore();
	//
	if (m_lData.empty()) {
		m_nStatus = IPI_STATUS_NODATATOSTORE;
		return false;
	}
	QFileInfo fiOrgFile(QString::fromStdString(m_sFile));
	QFileInfo fiOut(fiOrgFile.absolutePath(), fiOrgFile.baseName() + FILE_DOT_EXT(OIF_FILE_EXT));
	CDatabaseManager * cDM = new CDatabaseManager(NULL);
	cDM->SetType(CDatabaseManager::DBM_TYPE_SQLITE);
	cDM->SetName(fiOut.absoluteFilePath());
	if (cDM->Open() == false) {
		m_nStatus = IPI_STATUS_FAILEDOPENOIF;
		m_sError = QString("Error on file: %1.\n%2").arg(fiOut.absoluteFilePath())
			.arg(cDM->LastError()).toStdString();
		delete cDM;
		return false;
	}
	if (cDM->Init(this) == false) {
		m_nStatus = IPI_STATUS_FAILEDINITOIF;
		m_sError = cDM->LastError().toStdString();
		return false;
	}
	m_cStore = cDM;
	return true;
}

void * isl::CConnect::GetStore()
{
	return m_cStore;
}

bool isl::CConnect::CloseStore()
{
	bool bRet = true;
	if (m_cStore != NULL) {
		bRet = ((CDatabaseManager *)m_cStore)->Close();
		delete (CDatabaseManager *)m_cStore;
	}
	else {
		bRet = false;
	}
	m_cStore = NULL;
	return bRet;
}
#endif
