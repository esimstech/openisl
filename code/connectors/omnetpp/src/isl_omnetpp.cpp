/* 
 *     Name: isl_omnetpp.cpp
 *
 *     Description: OMNet++ coupling for OpenISL.
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
#include "isl_omnetpp.h"


/*
 *     Classes definition
 */

ISL_OMNETPP_EXPORT isl::CConnect * ISLInitialize(const char * sProcessName,
	const char * sSessionId, double dStartTime, double dEndTime, double dStepSize, int & nStatus)
{
	std::string sConnectID = std::string(sSessionId) + "_" + std::string(sProcessName);
	nStatus = 0;
	isl::CConnect * cConnect = ISLInstances->Get(sConnectID);
	if (cConnect != 0) {
		ISLLogInfo(INFO_MODEL_ALREADY_INSTANTIATED,
			"The connector '%s' has already been instantiated.", sConnectID.c_str());
		return cConnect;
	}
	cConnect = new isl::CConnect();
	cConnect->CloseLogOnDelete(true);
	if (cConnect->New(sProcessName) == false) {
		nStatus = ERROR_CREATEMODEL_FAILED;
		ISLLogError(nStatus, "Failed to create the new connector instance: %s", sProcessName);
		delete cConnect;
		return 0;
	}
	ISLLogInfo(99998, "DLL version: " GET_APP_VERSION(VERSION_NUMBER));
	cConnect->SetType("ONET");
	if (cConnect->SetSessionId(sSessionId) == false) {
		nStatus = ERROR_SETSESSIONID_FAILED;
		ISLLogError(nStatus, "Failed to set the session identifier: %s.", sSessionId);
		delete cConnect;
		return 0;
	}
	cConnect->SetStartTime(dStartTime);
	cConnect->SetEndTime(dEndTime);
	cConnect->SetStepSize(dStepSize);
	ISLLogInfo(INFO_MODEL_CREATED, "Model '%s' has been created.", cConnect->GetName().c_str());
	//
	ISLInstances->Add(cConnect, sConnectID);
	return cConnect;
}

ISL_OMNETPP_EXPORT int ISLConnect(const char * sName, const char * sSessionId)
{
	std::string sConnectID = std::string(sSessionId) + "_" + std::string(sName);
	isl::CConnect * cConnect = ISLInstances->Get(sConnectID);
	if (cConnect == 0) {
		ISLLogError(ERROR_CONNECT_NOMODEL, "An ISL model is required to connect.");
		return ERROR_CONNECT_NOMODEL;
	}
	if (cConnect->IsConnnected()) {
		return 0;
	}
	if (cConnect->Check() == false) {
		ISLLogError(ERROR_CHECKMODEL_FAILED,
			"Failed to check the connector '%s' configuration.", cConnect->GetName().c_str());
		return ERROR_CHECKMODEL_FAILED;
	}
	if (cConnect->Create() == false) {
		ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session: %s.",
			cConnect->GetSessionId().c_str());
		return ERROR_CREATESESSION_FAILED;
	}
	ISLLogInfo(INFO_SESSION_CREATED, "The session '%s' has been created.",
		cConnect->GetSessionId().c_str());
	if (cConnect->Connect(true) == false) {
		ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect the transmitters to the session '%s'.",
			cConnect->GetSessionId().c_str());
		return ERROR_CONNECT_FAILED;
	}
	ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
	//
	// TODO: Implement store
	//
	ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API: initialization done.");
	return 0;
}

ISL_OMNETPP_EXPORT int ISLTerminate(const char * sName, const char * sSessionId)
{
	std::string sConnectID = std::string(sSessionId) + "_" + std::string(sName);
	isl::CConnect * cConnect = ISLInstances->Get(sConnectID);
	if (cConnect == 0) {
		return 0;
	}
	//
	// TODO: Implement store
	//
	if (cConnect->Disconnect() == false) {
		ISLLogError(ERROR_DISCONNECT_FAILED, "Failed to disconnect from the ISL API.");
		ISLInstances->Delete(sConnectID);
		return ERROR_DISCONNECT_FAILED;
	}
	ISLLogInfo(INFO_DISCONNECTED, "Disconnected from the ISL API.");
	if (cConnect->SendStopSession() == false) {
		ISLLogWarning(WARNING_SENDSTOPEVENT, "Failed to send a stop event to connected apps.");
	}
	ISLLogInfo(INFO_ISLAPI_CLOSING, "ISL API: closing the simulation.");
	ISLInstances->Delete(sConnectID);
	return 0;
}

ISL_OMNETPP_EXPORT isl::CData * ISLAddRealIO(const char * sName, const char * sSessionId,
	const char * sId, const char * sConnectId, double dInitVal, bool bIsInput, int & nStatus)
{
	std::string sConnectID = std::string(sSessionId) + "_" + std::string(sName);
	isl::CConnect * cConnect = ISLInstances->Get(sConnectID);
	nStatus = 0;
	if (cConnect == 0) {
		nStatus = ERROR_ADDREALIO_NOMODEL;
		ISLLogError(nStatus, "The connector must be created first.");
		return 0;
	}
	isl::CVariable::tCausality eCausality = isl::CVariable::CS_OUTPUT;
	if (bIsInput) {
		eCausality = isl::CVariable::CS_INPUT;
	}
	isl::CData * cData = cConnect->NewIO(sId, eCausality, isl::CDataType::TP_REAL);
	if (cData == 0) {
		nStatus = ERROR_NEWIO_FAILED;
		ISLLogError(nStatus, "Failed to create the IO: %s.", sId);
		return 0;
	}
	cData->SetName(sId);
	cData->GetType()->Allocate();
	cData->GetType()->SetInitial(&dInitVal);
	cData->SetConnectId(sConnectId);
	return cData;
}

ISL_OMNETPP_EXPORT int ISLGetReal(isl::CData * cData, double dTime, double & dVal, double & dRetTime)
{
	try {
		if (cData->GetParent()->IsTerminated()) {
			ISLLogWarning(WARNING_GET_ENDOFSIM_REACHED, "End of simulation reached at time %gs.", dTime);
			return WARNING_GET_ENDOFSIM_REACHED;
		}
	}
	catch (...) {
		return ERROR_GET_NOACCESSTODATA;
	}
	try {
		double dLRetTime = dTime;
		double dLVal = 0.0;
		if (cData->GetData(&dLVal, &dLRetTime, dTime, true)) {
			//
			// TODO: Implement store
			//
			dVal = dLVal;
			dRetTime = dLRetTime;
		}
	}
	catch (...) {
		return ERROR_GETREAL_FAILED;
	}
	return 0;
}

ISL_OMNETPP_EXPORT int ISLSetReal(isl::CData * cData, double dTime, double dVal)
{
	try {
		if (cData->GetParent()->IsTerminated()) {
			ISLLogWarning(WARNING_SET_ENDOFSIM_REACHED, "End of simulation reached at time %gs.", dTime);
			return WARNING_SET_ENDOFSIM_REACHED;
		}
	}
	catch (...) {
		return ERROR_SET_NOACCESSTODATA;
	}
	try {
		cData->SetData(&dVal, dTime, true);
		//
		// TODO: Implement store
		//
	}
	catch (...) {
		return ERROR_SETREAL_FAILED;
	}
	return 0;
}

ISL_OMNETPP_EXPORT int ISLCheckAndSetReal(isl::CData * cData, double dTime, double dCurrentTime,
	double dStepSize, double dVal)
{
	if (dTime < dCurrentTime - dStepSize * 1e-6) {
		// Send data
		return ISLSetReal(cData, dCurrentTime, dVal);
	}
	return -1;
}
