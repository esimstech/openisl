/* 
 *     Name: isl_matlab.cpp
 *
 *     Description: Matlab coupling for ISL.
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

#include <map>
#include <string>
#include <isl_api.h>
#include "isl_matlab.h"
#include "logcodes.h"


/*
 *     Variables definition
 */

// TODO : Use ISLInstance instead of using global variables
static std::map<std::string, isl::CData *> g_mIOs;
static std::string g_sInstanceID;
static tStatusCode g_eStatusCode = ISL_MAT_SC_SUCCESS;


/*
 *     Functions definition
 */

ISL_MATLAB_EXPORT int ISLStatusCode()
{
	return (int)g_eStatusCode;
}

ISL_MATLAB_EXPORT bool ISLSetup(const char * sXMLFile)
{
	isl::CConnect * cConnect = ISLInstances->Get(g_sInstanceID);
	if (cConnect == 0) {
		if (sXMLFile == 0) {
			g_eStatusCode = ISL_MAT_SC_XMLFILE;
			return false;
		}
		cConnect = new isl::CConnect();
		cConnect->CloseLogOnDelete(true);
		cConnect->SetMode(isl::CConnect::SMD_STOP);
		// Load the XML file
		if (cConnect->Load(sXMLFile) == false) {
			ISLLogError(ERROR_LOADXML_FAILED, "Failed to load the file '%s'.", sXMLFile);
			delete cConnect;
			cConnect = 0;
			g_eStatusCode = ISL_MAT_SC_LOADXML;
			return false;
		}
		ISLLogInfo(INFO_XML_LOADED, "File '%s' loaded.", sXMLFile);
		// Create the session
		// The session identifier is loaded from the XML file
		if (cConnect->Create() == false) {
			ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session '%s'.",
				cConnect->GetSessionId().c_str());
			delete cConnect;
			g_eStatusCode = ISL_MAT_SC_CREATESESSION;
			return false;
		}
		ISLLogInfo(INFO_SESSION_CREATED, "Session '%s' has been created.",
			cConnect->GetSessionId().c_str());
		// Fill the I/Os map
		try {
			int nNbIOs = cConnect->GetNbIOs();
			for (int i = 0; i < nNbIOs; i++) {
				isl::CData * cIO = cConnect->GetIO(i);
				std::string sName = cIO->GetId();
				g_mIOs[sName] = cIO;
			}
		}
		catch (...) {
			g_eStatusCode = ISL_MAT_SC_MAPDATA;
			return false;
		}
		// Set the thread to listen on co-simulation stop
		if (cConnect->ListenToExitSession() == false) {
			ISLLogWarning(WARNING_LISTENTOEXITSESSION, "Call of ListenToExitSession failed");
		}
		// Connect with transmitters
		if (cConnect->Connect() == false) {
			ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect to the transmitters on session '%s'",
				cConnect->GetSessionId().c_str());
			cConnect->Disconnect();
			delete cConnect;
			g_eStatusCode = ISL_MAT_SC_CONNECTSESSION;
			return false;
		}
		ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
		//
		// TODO: Implements the ISL store
		//
		ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API: initialization done.");
		// g_sInstanceID will be set with a unique ID computed by the ISL API
		ISLInstances->Add(cConnect, g_sInstanceID);
	}
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	return true;
}

ISL_MATLAB_EXPORT bool ISLGetData(const char * sName, void * pData, double * dOutTime, double dTime, bool bWait)
{
	if (sName == 0) {
		g_eStatusCode = ISL_MAT_SC_DATAID;
		return false;
	}
	isl::CData * cData = g_mIOs[sName];
	if (cData == 0) {
		g_eStatusCode = ISL_MAT_SC_IOMAP;
		return false;
	}
	try {
		if (cData->GetParent()->IsTerminated()) {
			g_eStatusCode = ISL_MAT_SC_TERMINATED;
			return false;
		}
		if (cData->GetData(pData, dOutTime, dTime, bWait) == false) {
			g_eStatusCode = ISL_MAT_SC_NOTRECEIVED;
			return false;
		}
		//
		// TODO: Implements store data
		//
	}
	catch (...) {
		g_eStatusCode = ISL_MAT_SC_GETDATA;
		return false;
	}
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	return true;
}

ISL_MATLAB_EXPORT bool ISLGetDataLast(const char * sName, void * pData, double * dOutTime, bool bWait)
{
	if (sName == 0) {
		g_eStatusCode = ISL_MAT_SC_DATAID;
		return false;
	}
	isl::CData * cData = g_mIOs[sName];
	if (cData == 0) {
		g_eStatusCode = ISL_MAT_SC_IOMAP;
		return false;
	}
	try {
		if (cData->GetParent()->IsTerminated()) {
			g_eStatusCode = ISL_MAT_SC_TERMINATED;
			return false;
		}
		if (cData->GetData(pData, dOutTime, bWait) == false) {
			g_eStatusCode = ISL_MAT_SC_NOTRECEIVED;
			return false;
		}
		//
		// TODO: Implements store data
		//
	}
	catch (...) {
		g_eStatusCode = ISL_MAT_SC_GETDATA;
		return false;
	}
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	return true;
}

ISL_MATLAB_EXPORT bool ISLGetDataEvt(const char * sName, void * pData, bool bWait)
{
	if (sName == 0) {
		g_eStatusCode = ISL_MAT_SC_DATAID;
		return false;
	}
	isl::CData * cData = g_mIOs[sName];
	if (cData == 0) {
		g_eStatusCode = ISL_MAT_SC_IOMAP;
		return false;
	}
	try {
		if (cData->GetParent()->IsTerminated()) {
			g_eStatusCode = ISL_MAT_SC_TERMINATED;
			return false;
		}
		if (cData->GetEventData(pData, bWait) == false) {
			g_eStatusCode = ISL_MAT_SC_NOTRECEIVED;
			return false;
		}
		//
		// TODO: Implements store data
		//
	}
	catch (...) {
		g_eStatusCode = ISL_MAT_SC_GETDATA;
		return false;
	}
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	return true;
}

ISL_MATLAB_EXPORT bool ISLSetData(const char * sName, void * pData, double dTime, bool bWait)
{
	if (sName == 0) {
		g_eStatusCode = ISL_MAT_SC_DATAID;
		return false;
	}
	isl::CData * cData = g_mIOs[sName];
	if (cData == 0) {
		g_eStatusCode = ISL_MAT_SC_IOMAP;
		return false;
	}
	try {
		if (cData->GetParent()->IsTerminated()) {
			g_eStatusCode = ISL_MAT_SC_TERMINATED;
			return false;
		}
		if (cData->SetData(pData, dTime, bWait) == false) {
			g_eStatusCode = ISL_MAT_SC_NOTSENT;
			return false;
		}
		//
		// TODO: Implements store data
		//
	}
	catch (...) {
		g_eStatusCode = ISL_MAT_SC_SETDATA;
		return false;
	}
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	return true;
}

ISL_MATLAB_EXPORT bool ISLSetDataEvt(const char * sName, void * pData, bool bWait)
{
	if (sName == 0) {
		g_eStatusCode = ISL_MAT_SC_DATAID;
		return false;
	}
	isl::CData * cData = g_mIOs[sName];
	if (cData == 0) {
		g_eStatusCode = ISL_MAT_SC_IOMAP;
		return false;
	}
	try {
		if (cData->GetParent()->IsTerminated()) {
			g_eStatusCode = ISL_MAT_SC_TERMINATED;
			return false;
		}
		if (cData->SetEventData(pData, bWait) == false) {
			g_eStatusCode = ISL_MAT_SC_NOTSENT;
			return false;
		}
		//
		// TODO: Implements store data
		//
	}
	catch (...) {
		g_eStatusCode = ISL_MAT_SC_SETDATA;
		return false;
	}
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	return true;
}

ISL_MATLAB_EXPORT bool ISLTerminate()
{
	g_eStatusCode = ISL_MAT_SC_SUCCESS;
	isl::CConnect * cConnect = ISLInstances->Get(g_sInstanceID);
	if (cConnect == 0) {
		g_eStatusCode = ISL_MAT_SC_NOSESSION;
		return false;
	}
	//
	// TODO: Implements the ISL store
	//
	if (cConnect->Disconnect() == false) {
		ISLLogError(ERROR_DISCONNECT, "Failed to disconnect from the ISL API");
		g_eStatusCode = ISL_MAT_SC_DISCONNECT;
	}
	else {
		ISLLogInfo(INFO_DISCONNECTED, "Disconnected from the ISL API.");
	}
	if (cConnect->SendStopSession() == false) {
		ISLLogError(WARNING_SENDSTOPEVENT, "Failed to send a stop request to the connected components.");
	}
	ISLLogInfo(INFO_ISLAPI_CLOSING, "ISL API: closing the simulation.");
	//delete cConnect; // TODO : freeze Matlab
	//ISLInstances->Remove(g_sInstanceID);
	//g_mIOs.clear();
	if (g_eStatusCode != ISL_MAT_SC_SUCCESS) {
		return false;
	}
	return true;
}
