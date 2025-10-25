/* 
 *     Name: islfmuslave.cpp
 *
 *     Description: FMI2 Slave coupling for ISL.
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

#include <cstring>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>

#include "logcodes.h"
#include "v2.0/fmi2Functions.h"
#include "v2.0/fmi2FunctionTypes.h"
#include "islfmuslave.h"


/*
 *     Classes definition
 */


// FMI constants
#define FMI_PARAM_WORKDIR 90000
#define FMI_PARAM_XMLFILE 90001
#define FMI_PARAM_SESSION 90002


ISLFMUSlave * ISLFMUSlave::Create() {
	return new ISLFMUSlave;
}

ISLFMUSlave::ISLFMUSlave()
{
	m_cConnect = 0;
	m_callbackFunctions = 0;
	m_initializationMode = false;
	m_isCosimulation = false;
	m_loggingOn = false;
	m_currentTimePoint = 0.0;
	m_alreadyCleaned = false;
}


ISLFMUSlave::~ISLFMUSlave() {
	ISLTerminate();
}


// create a model instance
void ISLFMUSlave::Init() {
	Logger(fmi2OK, "progress", "Starting initialization.");
	//
	// Check variables validity
	if (m_stringVar[FMI_PARAM_WORKDIR].empty()) {
		throw std::runtime_error("Invalid working directory parameter.");
		return;
	}
	if (m_stringVar[FMI_PARAM_XMLFILE].empty()) {
		throw std::runtime_error("Invalid FMX file parameter.");
		return;
	}
	if (m_stringVar[FMI_PARAM_SESSION].empty()) {
		throw std::runtime_error("Invalid ISL session parameter.");
		return;
	}

	if (m_isCosimulation == false) {
		throw std::runtime_error("Invalid mode: ModelExchange not supported.");
		return;
	}
	//
	// Co-simulation mode
	if (ISLSetup() == false) {
		throw std::runtime_error("ISL Setup failed. Check the ISL log file for the details.");
		return;
	}
	// Check if the model is already instantiated
	// initialize integrator for co-simulation
	m_currentTimePoint = 0.0;

	Logger(fmi2OK, "progress", "ISL Initialization complete.");
}


void ISLFMUSlave::EndInit() {
	//
	// Initialize all acceptable variables
	for (CISLFMUDataMap::iterator iElt = m_mIOs.begin(); iElt != m_mIOs.end(); ++iElt) {
		isl::CData * cData = iElt->second->m_cData;
		if (cData->GetType()) {
			if (cData->GetType()->IsInitialSet()) {
				void * m_pInitial = cData->GetType()->GetInitial();
				if (m_pInitial) {
					std::string msg = "Initializing " + iElt->second->m_sName + "[";
					msg += std::to_string(iElt->second->m_uRef) + "] = ";
					switch (iElt->second->m_eType) {
						case isl::CDataType::TP_REAL:
							m_realVar[iElt->second->m_uRef] = *(double *)m_pInitial;
							msg += std::to_string(m_realVar[iElt->second->m_uRef]);
							break;
						case isl::CDataType::TP_INTEGER:
							m_integerVar[iElt->second->m_uRef] = *(int *)m_pInitial;
							msg += std::to_string(m_integerVar[iElt->second->m_uRef]);
							break;
						default:
							break;
					}
					Logger(fmi2OK, "progress", msg);
				}
			}
		}
	}
}

void ISLFMUSlave::Terminate() {
	ISLTerminate();
}


void ISLFMUSlave::SetReal(int varID, double value) {
	try {
		m_realVar[varID] = value;
	}
	catch (...) {
		std::stringstream strm;
		strm << "SetReal failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::SetInt(int varID, int value) {
	try {
		m_integerVar[varID] = value;
	}
	catch (...) {
		std::stringstream strm;
		strm << "SetInt failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::SetString(int varID, fmi2String value) {
	try {
		m_stringVar[varID] = value;
	}
	catch (...) {
		std::stringstream strm;
		strm << "SetString failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::SetBool(int varID, bool value) {
	try {
		m_boolVar[varID] = value;
	}
	catch (...) {
		std::stringstream strm;
		strm << "SetBool failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::GetReal(int varID, double& value) {
	try {
		value = m_realVar[varID];
	}
	catch (...) {
		std::stringstream strm;
		strm << "GetReal failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::GetInt(int varID, int& value) {
	try {
		value = m_integerVar[varID];
	}
	catch (...) {
		std::stringstream strm;
		strm << "GetInt failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::GetString(int varID, fmi2String& value) {
	try {
		value = m_stringVar[varID].c_str();
	}
	catch (...) {
		std::stringstream strm;
		strm << "GetString failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


void ISLFMUSlave::GetBool(int varID, bool& value) {
	try {
		value = m_boolVar[varID];
	}
	catch (...) {
		std::stringstream strm;
		strm << "GetBool failed. Invalid or unknown value reference " << varID;
		throw std::runtime_error(strm.str());
	}
}


// Co-simulation: time integration
void ISLFMUSlave::IntegrateTo(double currentCommunicationPoint, double communicationStepSize) {
	// Set the current time point
	m_currentTimePoint = currentCommunicationPoint;
	// Check if ISL co-simulation is terminated
	if (m_cConnect->IsTerminated()) {
		throw std::runtime_error("ISL co-simulation ended.");
		return;
	}
	// Get FMU inputs and send values on FMX outputs
	if (GetDataAndSend() == false) {
		throw std::runtime_error(std::string("GetDataAndSend(): ") + m_sErrorMsg);
	}
	// Received FMX inputs and set FMU outputs
	if (ReceiveAndSetData() == false) {
		throw std::runtime_error(std::string("ReceiveAndSetData(): ") + m_sErrorMsg);
	}
	// Not useful
	m_currentTimePoint = currentCommunicationPoint + communicationStepSize;
}

void ISLFMUSlave::Logger(fmi2Status state, fmi2String category, fmi2String msg)
{
	if (m_loggingOn) {
		m_callbackFunctions->logger(m_callbackFunctions->componentEnvironment,
			m_instanceName.c_str(), state, category, msg);
	}
}

void ISLFMUSlave::Logger(fmi2Status state, fmi2String category, const std::string& msg)
{
	m_lastError = msg;
	Logger(state, category, m_lastError.c_str());
}


bool ISLFMUSlave::ISLSetup()
{
	bool bFillTheMap = false;
	std::string sSession = m_stringVar[FMI_PARAM_SESSION];
	if (m_cConnect == 0) {
		m_cConnect = ISLInstances->Get(m_GUID);
		bFillTheMap = true;
	}
	if (m_cConnect == 0) {
		//
		boost::filesystem::path bfpInFile(m_stringVar[FMI_PARAM_WORKDIR]);
		bfpInFile.append(m_stringVar[FMI_PARAM_XMLFILE]);
		//
		m_cConnect = new isl::CConnect();
		m_cConnect->SetMode(isl::CConnect::SMD_STOP);
		m_cConnect->CloseLogOnDelete(true);
		// Load the XML file
		if (m_cConnect->Load(bfpInFile.string()) == false) {
			ISLLogError(ERROR_LOADXML_FAILED, "Failed to load the file '%s'.", bfpInFile.string().c_str());
			delete m_cConnect;
			m_cConnect = 0;
			return false;
		}
		ISLLogInfo(99998, "DLL version: %s", GET_APP_VERSION(VERSION_NUMBER));
		ISLLogInfo(INFO_XML_LOADED, "File '%s' loaded.", bfpInFile.string().c_str());
		// Set session identifier if not empty
		if (sSession.empty() == false) {
			if (m_cConnect->SetSessionId(sSession) == false) {
				ISLLogWarning(WARNING_WRONG_SESSIONID, "The session id '%s' has not been set.", sSession.c_str());
			}
			else {
				ISLLogInfo(INFO_SESSIONID_SET, "The session id has been set to: ", sSession.c_str());
			}
		}
		// Create the session
		if (m_cConnect->Create() == false) {
			ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session '%s'",
				m_cConnect->GetSessionId().c_str());
			delete m_cConnect;
			m_cConnect = 0;
			return false;
		}
		ISLLogInfo(INFO_SESSION_CREATED, "Session '%s' has been created.", m_cConnect->GetSessionId().c_str());
		// Set the thread to listen on co-simulation stop
		if (m_cConnect->ListenToExitSession() == false) {
			ISLLogWarning(WARNING_LISTENTOEXITSESSION, "Call of ListenToExitSession failed");
		}
		// Connect with transmitters
		if (m_cConnect->Connect() == false) {
			ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect to the transmitters on session %s",
				m_cConnect->GetSessionId().c_str());
			m_cConnect->Disconnect();
			delete m_cConnect;
			m_cConnect = 0;
			return false;
		}
		ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
		//
		// TODO: Implements the ISL store
		//
		bFillTheMap = true;
		ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API initialization done.");
		ISLInstances->Add(m_cConnect, m_GUID);
	}
	if (bFillTheMap) {
		// Fill the map
		try {
			int nNbIOs = m_cConnect->GetNbIOs();
			for (int i = 0; i < nNbIOs; i++) {
				isl::CData * cData = m_cConnect->GetIO(i);
				std::string sName = cData->GetId();
				unsigned int uRef = boost::lexical_cast<unsigned int>(cData->GetName());
				ISLFMUData * cISLData = new ISLFMUData;
				cISLData->m_cData = cData;
				cISLData->m_sName = sName;
				cISLData->m_uRef = uRef;
				cISLData->m_eType = cData->GetType()->GetId();
				m_mIOs[(int)uRef] = cISLData;
				if (cData->IsInput()) {
					m_lInRefs.push_back((int)uRef);
				}
				else if (cData->IsOutput()) {
					m_lOutRefs.push_back((int)uRef);
				}
			}
		}
		catch (...) {
			ISLLogError(ERROR_MAPIOS_FAILED, "Exception raised when loading FMX IOs.");
			return false;
		}
	}
	return true;
}

bool ISLFMUSlave::ISLTerminate()
{
	if (m_alreadyCleaned) {
		return true;
	}
	m_alreadyCleaned = true;
	if (m_cConnect == 0) {
		m_cConnect = ISLInstances->Get(m_GUID);
	}
	if (m_cConnect == 0) {
		return false;
	}
	//
	// TODO: Implements the ISL store
	//
	bool bRet = true;
	if (m_cConnect->Disconnect() == false) {
		ISLLogError(ERROR_DISCONNECT_FAILED, "Failed to disconnect from the ISL API");
		bRet = false;
	}
	else {
		ISLLogInfo(INFO_SESSION_DISCONNECTED, "Disconnected from the ISL API.");
	}
	if (m_cConnect->SendStopSession() == false) {
		ISLLogWarning(WARNING_FAILED_SENDSTOPEVENT,
			"Failed to send a stop request to the connected components.");
	}
	ISLLogInfo(INFO_ISLAPI_CLOSING, "ISL API: closing the simulation.");
	m_cConnect->CloseLogOnDelete(false);
	delete m_cConnect;
	m_cConnect = 0;
	// Clean map
	try {
		CISLFMUDataMap::iterator iElt = m_mIOs.begin();
		while (iElt != m_mIOs.end()) {
			delete iElt->second;
			iElt = m_mIOs.erase(iElt);
		}
		ISLLogInfo(INFO_MAPIOS_CLEANED, "ISL API: Signals map cleaned.");
	}
	catch (...) {
		ISLLogError(ERROR_CLEAN_MAPIOS, "ISL API: Error when clean signals map.");
		bRet = false;
	}
	ISLLogClose();
	return bRet;
}

bool ISLFMUSlave::GetDataAndSend()
{
	bool bRet = true;
	for (int i = 0; i < m_lOutRefs.size(); i++) {
		int nRef = m_lOutRefs[i];
		if (m_mIOs.find(nRef) == m_mIOs.end()) {
			m_sErrorMsg = std::string("No ISL output referenced by: ") +
				std::to_string(nRef);
			bRet = false;
			continue;
		}
		ISLFMUData * cData = m_mIOs[nRef];
		void * pVal = 0;
		switch (cData->m_eType) {
			case isl::CDataType::TP_REAL:
				pVal = new double;
				*(double *)pVal = m_realVar[nRef];
				break;
			case isl::CDataType::TP_INTEGER:
				pVal = new int;
				*(int *)pVal = m_integerVar[nRef];
				break;
			default:
				break;
		}
		if (pVal != 0) {
			if (cData->m_cData->SetData(pVal, m_currentTimePoint, true) == false) {
				m_sErrorMsg = std::string("Failed to send output: ") +
					std::to_string(nRef);
				bRet = false;
			}
			//
			// TODO: Implements store data
			//
			delete pVal;
			pVal = 0;
		}
	}
	return bRet;
}

bool ISLFMUSlave::ReceiveAndSetData()
{
	bool bRet = true;
	for (int i = 0; i < m_lInRefs.size(); i++) {
		int nRef = m_lInRefs[i];
		if (m_mIOs.find(nRef) == m_mIOs.end()) {
			m_sErrorMsg = std::string("No ISL input referenced by: ") +
				std::to_string(nRef);
			bRet = false;
			continue;
		}
		ISLFMUData * cData = m_mIOs[nRef];
		void * pVal = 0;
		switch (cData->m_eType) {
			case isl::CDataType::TP_REAL:
				pVal = new double;
				*(double *)pVal = m_realVar[nRef];
				break;
			case isl::CDataType::TP_INTEGER:
				pVal = new int;
				*(int *)pVal = m_integerVar[nRef];
				break;
			default:
				break;
		}
		if (pVal != 0) {
			double dOutTime;
			if (cData->m_cData->GetData(pVal, &dOutTime, m_currentTimePoint, true) == false) {
				m_sErrorMsg = std::string("Failed to receive input: ") +
					std::to_string(nRef);
				bRet = false;
			}
			//
			// TODO: Implements store data
			//
		}
		switch (cData->m_eType) {
		case isl::CDataType::TP_REAL:
			m_realVar[nRef] = *(double *)pVal;
			break;
		case isl::CDataType::TP_INTEGER:
			m_integerVar[nRef] = *(int *)pVal;
			break;
		default:
			break;
		}
		if (pVal != 0) {
			delete pVal;
			pVal = 0;
		}
	}
	return bRet;
}


