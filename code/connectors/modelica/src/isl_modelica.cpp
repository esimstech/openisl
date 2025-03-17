/* 
 *     Name: tts_modelica.cpp
 *
 *     Description: Modelica coupling for ISL.
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

#include <isl_api.h>

#include "logcodes.h"
#include "isl_modelica.h"


/*
 *     Macros definition
 */

#if 1
#define LOCAL_DEBUG
#endif

#define DEFAULT_STEP_SIZE	1e-3


/*
 *     Variables definition
 */

// Improve the library
// No XML file
// No global variable
ISL_MODELICA_EXPORT void * g_cConnect = 0;


/*
 *     Functions definition
 */

ISL_MODELICA_EXPORT int ISLInitialize(int nISLId, const char * sXMLFile, const char * sSession, const char * sName)
{
	isl::CConnect * cConnect;
	if (g_cConnect == 0) {
		if (sXMLFile == 0) {
			return -1;
		}
		if (sSession == 0) {
			return -2;
		}
		cConnect = new isl::CConnect();
		cConnect->CloseLogOnDelete(true);
		// Load the XML file
		if (cConnect->Load(sXMLFile) == false) {
			ISLLogError(ERROR_LOADXML_FAILED, "Failed to load the file '%s'.", sXMLFile);
			delete cConnect;
			return -3;
		}
		ISLLogInfo(99998, "DLL version: %s", GET_APP_VERSION(VERSION_NUMBER));
		ISLLogInfo(INFO_XML_LOADED, "File '%s' loaded.", sXMLFile);
		// Set session identifier
		if (cConnect->SetSessionId(sSession) == false) {
			ISLLogWarning(WARNING_WRONG_SESSIONID, "The session id '%s' has not been set.", sSession);
		}
		else {
			ISLLogInfo(INFO_SESSIONID_SET, "The session id has been set to: ", sSession);
		}
		// Create the session
		if (cConnect->Create() == false) {
			ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session '%s'",
				cConnect->GetSessionId().c_str());
			delete cConnect;
			return -5;
		}
		ISLLogInfo(INFO_SESSION_CREATED, "Session '%s' has been created.", cConnect->GetSessionId().c_str());
		// Set the thread to listen on co-simulation stop
		if (cConnect->ListenToExitSession() == false) {
			ISLLogWarning(WARNING_LISTENTOEXITSESSION, "Call of ListenToExitSession failed");
		}
		// Connect with transmitters
		if (cConnect->Connect() == false) {
			ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect to the transmitters on session %s",
				cConnect->GetSessionId().c_str());
			cConnect->Disconnect();
			delete cConnect;
			return -6;
		}
		ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
		//
		// TODO: Implements the ISL store
		//
		ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API: initialization done.");
		g_cConnect = cConnect;
	}
	else {
		cConnect = (isl::CConnect *)g_cConnect;
	}
	int nInd = -21;
	isl::CData * cData = cConnect->GetIO(sName, &nInd);
	if (cData == 0) {
		return -20;
	}
	return nInd;
}

ISL_MODELICA_EXPORT double ISLGetStep(int nId)
{
	if (nId < 0) {
		return DEFAULT_STEP_SIZE;
	}
	if (g_cConnect == 0) {
		return DEFAULT_STEP_SIZE;
	}
	isl::CConnect * cConnect = (isl::CConnect *)g_cConnect;
	isl::CData * cData = cConnect->GetIO(nId);
	if (cData != 0) {
		return cData->GetOriginalStep();
	}
	return DEFAULT_STEP_SIZE;
}

ISL_MODELICA_EXPORT double ISLCGetReal(int nId, double dTime, int nEnd)
{
	if (nId < 0) {
		return 0.0;
	}
	if (g_cConnect == 0) {
		return 0.0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)g_cConnect;
	if (nEnd == 1) {
		//
		// TODO: Implements the ISL store
		//
		if (cConnect->Disconnect() == false) {
			ISLLogError(ERROR_GETREAL_DISCONNECT, "Failed to disconnect from the ISL API");
		}
		else {
			ISLLogInfo(INFO_GETREAL_DISCONNECTED, "Disconnected from the ISL API.");
		}
		if (cConnect->SendStopSession() == false) {
			ISLLogWarning(WARNING_GETREAL_SENDSTOPEVENT,
				"Failed to send a stop request to the connected components.");
		}
		ISLLogInfo(INFO_GETREAL_ISLAPI_CLOSING, "ISL API: closing the simulation.");
		delete cConnect;
		g_cConnect = 0;
		return 0.0;
	}
#ifdef LOCAL_DEBUG
	ISLLogDebug(1, DEBUG_GETREAL_TIME_DATA, "Time: %gs, Get data[%d]", dTime, nId);
#endif
	isl::CData * cData = cConnect->GetIO(nId);
	if (cData != 0) {
		double dGivenTime = 0.0;
		double dVal = 0.0;
		if (cData->GetData(&dVal, &dGivenTime, dTime, true)) {
			//
			// TODO: Implements store data
			//
			return dVal;
		}
	}
	return 0.0;
}

ISL_MODELICA_EXPORT int ISLCSetReal(int nId, double dVal, double dTime, int nEnd)
{
	if (nId < 0) {
		return -1;
	}
	if (g_cConnect == 0) {
		return -2;
	}
	isl::CConnect * cConnect = (isl::CConnect *)g_cConnect;
	if (nEnd == 1) {
		//
		// TODO: Implements the ISL store
		//
		if (cConnect->Disconnect() == false) {
			ISLLogError(ERROR_SETREAL_DISCONNECT, "Failed to disconnect from the ISL API");
		}
		else {
			ISLLogInfo(INFO_SETREAL_DISCONNECTED, "Disconnected from the ISL API.");
		}
		if (cConnect->SendStopSession() == false) {
			ISLLogWarning(WARNING_SETREAL_SENDSTOPEVENT,
				"Failed to send a stop request to the connected components.");
		}
		ISLLogInfo(INFO_SETREAL_ISLAPI_CLOSING, "ISL API: closing the simulation.");
		delete cConnect;
		g_cConnect = 0;
		return -3;
	}
#ifdef LOCAL_DEBUG
	ISLLogDebug(1, DEBUG_SETREAL_TIME_DATA,
		"Time: %gs, Set data[%d]=%g", dTime, nId, dVal);
#endif
	isl::CData * cData = cConnect->GetIO(nId);
	if (cData != 0) {
		if (cData->SetData(&dVal, dTime, true)) {
			//
			// TODO: Implements store data
			//
			return dVal;
		}
	}
	return -4;
}
