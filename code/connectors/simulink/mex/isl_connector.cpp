/*
 *     Name: isl_connector.cpp
 *
 *     Description: ISL Connector S-Function.
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
 *     Macros definition
 */

// Debug ISL
#if 1
#define DEBUG_ISL
#endif
// Simulink Macros
// S-Function name and level
#define S_FUNCTION_NAME		isl_connector
#define S_FUNCTION_LEVEL	2

// Macros relative to the S-Function interface
// Number of parameters of the S-Function
#define SFUNC_PARAMS_NB 	4


/*
 *     Header files
 */

#include <simstruc.h>
#include <isl_api.h>
#include "logcodes.h"


/*
 *     Constants definition
 */

enum {
	PARAM_XML_FILE = 0,		// Param[0] = XML file
	PARAM_STORE_OPTION,		// Param[1] = Store option
	PARAM_DONOT_CONNECT,	// Param[2] = Do not connect option
	PARAM_INITIAL_STEP		// Param[3] = Initial Step size
};


/*
 *     Types definition
 */

typedef struct {
	bool m_bDoNotCosim;
	double m_dISLStep;
	double m_dStep;
	double m_dInitialStep;
	double m_dLastTime;
	isl::CConnect * m_cConnect;
} tISLInfo;


/*
 *     Simulink callback functions definition
 */

static void mdlInitializeSizes(SimStruct * S) {
	// Create a new instance of the structure
	tISLInfo * stInfo = new tISLInfo;
	stInfo->m_bDoNotCosim = false;
	stInfo->m_dISLStep = INHERITED_SAMPLE_TIME;
	stInfo->m_dStep = INHERITED_SAMPLE_TIME;
	stInfo->m_dInitialStep = 1e-6;
	stInfo->m_dLastTime = 0.0;
	stInfo->m_cConnect = NULL;
	// Set the user data to the structure above
	ssSetUserData(S, stInfo);
	// Set the number of parameters of the S-Function
	ssSetNumSFcnParams(S, SFUNC_PARAMS_NB);
	// Check that if the number is consistent with Simulink blocks
	if (ssGetNumSFcnParams(S) != ssGetSFcnParamsCount(S)) {
		mexErrMsgIdAndTxt("ISL:WrongSParameters", "Wrong number of S-Function parameters.");
		return;
	}
	// Set the number of states
	ssSetNumContStates(S, 0);
	ssSetNumDiscStates(S, 1);
	// Get the XML file
	size_t nSizeStr = mxGetN(ssGetSFcnParam(S, PARAM_XML_FILE)) * (sizeof(mxChar) + 1);
	if (nSizeStr <= 1) {
		return;
	}
	char * sXMLFile = (char *)mxMalloc(nSizeStr);
	int nStatus = mxGetString(ssGetSFcnParam(S, PARAM_XML_FILE), sXMLFile, (int )nSizeStr);
	if (nStatus != 0) {
		mexErrMsgIdAndTxt("ISL:WrongXMLFile", "Error on the XML file name. Error: %d.", nStatus);
		return;
	}
	// Load the XML file
	isl::CConnect * cConnect = new isl::CConnect();
	cConnect->CloseLogOnDelete(true);
	if (cConnect->Load(sXMLFile) == false) {
		mxFree(sXMLFile);
		std::string sFile(cConnect->GetFileName());
		ISLLogError(ERROR_LOADXML_FAILED, "Failed to load the XML file '%s'.",
			sFile.c_str());
		delete cConnect;
		mexErrMsgIdAndTxt("ISL:XMLLoadFailed", "Failed to load the XML file '%s'.",
			sFile.c_str());
		return;
	}
	mxFree(sXMLFile);
	ISLLogInfo(INFO_XML_LOADED, "XML file '%s' loaded.",
			cConnect->GetFileName().c_str());
	// Check the ISL structure
	if (cConnect->Check() == false) {
		ISLLogError(ERROR_CHECK_FAILED, "Failed to check to the ISL data structure.");
		delete cConnect;
		mexErrMsgIdAndTxt("ISL:CheckFailed", "Failed to check to the ISL data structure.");
		return;
	}
	// Check the number of IOs
	int nNbIOs = cConnect->GetNbIOs();
	if (nNbIOs <= 0) {
		ISLLogWarning(WARNING_NOIOSTOCONNECT, "No variable to connect.");
		mexWarnMsgIdAndTxt("ISL:NoVariable", "No variable to connect.");
		stInfo->m_bDoNotCosim = true;
	}
	// Set the number of inputs of the block, i.e. outputs of the model
	int nNbOutputs = cConnect->GetNbOutputs();
	if (!ssSetNumInputPorts(S, nNbOutputs)) {
		ISLLogError(ERROR_SETNBOUTPUTS_FAILED, "Failed to set the number of inputs of the block.");
		delete cConnect;
		mexErrMsgIdAndTxt("ISL:WrongInputsNumber", "Failed to set the number of inputs of the block.");
		return;
	}
	for (int_T i = 0; i < nNbOutputs; i++) {
		ssSetInputPortWidth(S, i, DYNAMICALLY_SIZED);
		ssSetInputPortDirectFeedThrough(S, i, 1);
		ssSetInputPortRequiredContiguous(S, i, 1);
	}
	// Set the number of outputs of the block, i.e. inputs of the model
	int_T nNbInputs = cConnect->GetNbInputs();
	if (!ssSetNumOutputPorts(S, nNbInputs)) {
		ISLLogError(ERROR_SETNBINPUTS_FAILED, "Failed to set the number of outputs of the block.");
		delete cConnect;
		mexErrMsgIdAndTxt("ISL:WrongOUTPortsNumber", "Failed to set the number of outputs of the block.");
		return;
	}
	for (int_T i = 0; i < nNbInputs; i++) {
		ssSetOutputPortWidth(S, i, DYNAMICALLY_SIZED);
	}
	//
	ssSetNumSampleTimes(S, 1);
	ssSetNumRWork(S, 0);
	ssSetNumIWork(S, 0);
	ssSetNumPWork(S, 0);
	ssSetNumModes(S, 0);
	ssSetNumNonsampledZCs(S, 0);
	//
	stInfo->m_cConnect = cConnect;
	//
	ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE
		| SS_OPTION_PLACE_ASAP
		| SS_OPTION_CALL_TERMINATE_ON_EXIT);
}

static void mdlInitializeSampleTimes(SimStruct * S)
{
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLInfo * stInfo = (tISLInfo *)pData;
	if (stInfo->m_cConnect == NULL) {
		return;
	}
	// Do not connect
	int nDoNotConnect = (int )mxGetPr(ssGetSFcnParam(S, PARAM_DONOT_CONNECT))[0];
	// Check the simulation status and mode
	if (nDoNotConnect == 1) {
		stInfo->m_bDoNotCosim = true;
	}
	SS_SimMode eMode = ssGetSimMode(S);
	if (eMode != SS_SIMMODE_NORMAL) {
		ISLLogInfo(INFO_SIMULATION_MODE, "Simulation mode: %d.", eMode);
		stInfo->m_bDoNotCosim = true;
	}
#ifndef SS_SIMSTATUS_NOT_DEFINED
	SS_SimStatus eStatus;
	ssGetSimStatus(S, &eStatus);
	if (eStatus != SIMSTATUS_INITIALIZING) {
		ISLLogInfo(INFO_SIMULATION_STATUS, "Simulation status: %d.", eStatus);
		stInfo->m_bDoNotCosim = true;
	}
#endif // SS_SIMSTATUS_NOT_DEFINED
	// Initial step
	stInfo->m_dInitialStep = (double )mxGetPr(ssGetSFcnParam(S, PARAM_INITIAL_STEP))[0];
	if (stInfo->m_dInitialStep < 0.0) {
		stInfo->m_dInitialStep = 1e-6;
	}
	// Identify the step size
	stInfo->m_dStep = INHERITED_SAMPLE_TIME;
	if (stInfo->m_bDoNotCosim == false) {
		// Get step size
		double dStep = stInfo->m_cConnect->GetStepSize();
		if (dStep > 0.0) {
			stInfo->m_dStep = dStep;
			stInfo->m_dISLStep = stInfo->m_dStep;
		}
	}
	ssSetSampleTime(S, 0, stInfo->m_dStep);
	ssSetOffsetTime(S, 0, 0.0);
}

#define MDL_INITIALIZE_CONDITIONS
static void mdlInitializeConditions(SimStruct * S)
{
}

#define MDL_START
static void mdlStart(SimStruct * S)
{
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLInfo * stInfo = (tISLInfo *)pData;
	if (stInfo->m_cConnect == NULL) {
		return;
	}
	if (stInfo->m_bDoNotCosim) {
		return;
	}
	// Activate the timer
	stInfo->m_cConnect->StartTimer();
	// Create the session
	// The session identifier is loaded from the XML file
	if (stInfo->m_cConnect->Create() == false) {
		ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session '%s'.",
			stInfo->m_cConnect->GetSessionId().c_str());
		delete stInfo->m_cConnect;
		stInfo->m_cConnect = NULL;
		mexErrMsgIdAndTxt("ISL:FailedCreateSession", "Failed to create the session '%s'.",
			stInfo->m_cConnect->GetSessionId().c_str());
		return;
	}
	ISLLogInfo(INFO_SESSION_CREATED, "Session '%s' has been created.",
		stInfo->m_cConnect->GetSessionId().c_str());
	// Listen to a stop signal
	if (stInfo->m_cConnect->ListenToExitSession() == false) {
		ISLLogWarning(WARNING_LISTENTOEXITSESSION, "Call of ListenToExitSession failed.");
		mexWarnMsgIdAndTxt("ISL:ListenFailed", "Call of ListenToExitSession failed.");
	}
	// Establish the connection
	if (stInfo->m_cConnect->Connect() == false) {
		ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect to the transmitters on session '%s'.",
			stInfo->m_cConnect->GetSessionId().c_str());
		delete stInfo->m_cConnect;
		stInfo->m_cConnect = NULL;
		mexErrMsgIdAndTxt("ISL:FailedToConnect", "Failed to connect to the transmitters on session '%s'.",
			stInfo->m_cConnect->GetSessionId().c_str());
		return;
	}
	ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
	// Store data
	int nStore = (int )mxGetPr(ssGetSFcnParam(S, PARAM_STORE_OPTION))[0];
	if (nStore == 1) {
		mexWarnMsgIdAndTxt("ISL:StoreNotImplemented", "Data storage not yet implemented.");
		//
		// TODO: Implements the ISL store
		//
	}
	ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API: Initialization done.");
	stInfo->m_dLastTime = ssGetTStart(S);
}

static void mdlOutputs(SimStruct * S, int_T nTid)
{
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLInfo * stInfo = (tISLInfo *)pData;
	if (stInfo->m_cConnect == NULL) {
		return;
	}
	if (stInfo->m_bDoNotCosim) {
		return;
	}
	if (stInfo->m_cConnect->IsTerminated()) {
		ISLLogInfo(INFO_STOPREQUESTED, "A stop request has been received.");
		ssSetStopRequested(S, 1);
		return;
	}
	double dTime = ssGetT(S);
	if (stInfo->m_dISLStep > 0.0) {
		stInfo->m_dStep = stInfo->m_dISLStep;
	}
	else {
		stInfo->m_dStep = dTime - stInfo->m_dLastTime;
		if (stInfo->m_dStep <= 0.0) {
			stInfo->m_dStep = stInfo->m_dISLStep;
		}
	}
	if (stInfo->m_dStep < 0.0) {
		stInfo->m_dStep = stInfo->m_dInitialStep;
	}
	bool bStore = (int)mxGetPr(ssGetSFcnParam(S, PARAM_STORE_OPTION))[0] == 1;
#ifdef DEBUG_ISL
	ISLLogDebug(1, DEBUG_CONNECTOR_STEP, "Time:%gs, last time:%gs, Step: %gs, ISL Step:%gs.",
		dTime, stInfo->m_dLastTime, stInfo->m_dStep, stInfo->m_dISLStep);
#endif // DEBUG_ISL
	stInfo->m_dLastTime = dTime;
	// Write the outputs of the model
	int nNbOutputs = stInfo->m_cConnect->GetNbOutputs();
	if (nNbOutputs > 0) {
		for (int i = 0; i < nNbOutputs; i++) {
			isl::CData * cData = stInfo->m_cConnect->GetOutput(i);
			if  (cData != NULL) {
				real_T * dVal = (real_T * )ssGetInputPortRealSignal(S, i);
#ifdef DEBUG_ISL
				ISLLogDebug(1, DEBUG_CONNECTOR_WRITE_DATA, "Writing port '%s':%g at time %gs.",
					cData->GetName().c_str(), *dVal, dTime);
#endif // DEBUG_ISL
				cData->SetData(dVal, dTime, stInfo->m_dStep, true);
				if (bStore || cData->IsStoreUsed()) {
					cData->StoreData(dVal, dTime);
				}
#ifdef DEBUG_ISL
				ISLLogDebug(1, DEBUG_CONNECTOR_WRITE_DONE, "Data written.");
#endif // DEBUG_ISL
				if (stInfo->m_cConnect->IsTerminated()) {
					ISLLogInfo(INFO_STOPREQUESTED, "A stop request has been received.");
					ssSetStopRequested(S, 1);
					return;
				}
			}
		}
	}
	// Read the inputs of the model
	int nNbInputs = stInfo->m_cConnect->GetNbInputs();
	if (nNbInputs > 0) {
		for (int i = 0; i < nNbInputs; i++) {
			isl::CData * cData = stInfo->m_cConnect->GetInput(i);
			if (cData != NULL) {
				real_T * dVal  = ssGetOutputPortRealSignal(S, i);
				double dOutTime = ssGetT(S);
#ifdef DEBUG_ISL
				ISLLogDebug(1, DEBUG_CONNECTOR_READ_DATA, "Reading port '%s' at time %gs.",
					cData->GetName().c_str(), dTime);
#endif // DEBUG_ISL
				cData->GetData(dVal, &dOutTime, dTime, true);
				if (bStore || cData->IsStoreUsed()) {
					cData->StoreData(dVal, dOutTime);
				}
#ifdef DEBUG_ISL
				ISLLogDebug(1, DEBUG_CONNECTOR_READ_DONE, "Data read:%g at time %gs.",
					*dVal, dOutTime);
#endif // DEBUG_ISL
				if (stInfo->m_cConnect->IsTerminated()) {
					ISLLogInfo(INFO_STOPREQUESTED, "A stop request has been received.");
					ssSetStopRequested(S, 1);
					return;
				}
			}
		}
	}
}

static void mdlDerivatives(SimStruct * S)
{
}

#define MDL_UPDATE
static void mdlUpdate(SimStruct * S, int_T tid)
{
}

static void mdlTerminate(SimStruct * S)
{
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLInfo * stInfo = (tISLInfo *)pData;
	if (stInfo->m_cConnect == NULL) {
		return;
	}
	int nStore = (int)mxGetPr(ssGetSFcnParam(S, PARAM_STORE_OPTION))[0];
	if (nStore == 1) {
		mexWarnMsgIdAndTxt("ISL:StoreNotImplemented", "Data storage not yet implemented.");
		//
		// TODO: Implements store data
		//
	}
	if (stInfo->m_cConnect->Disconnect() == false) {
		if (stInfo->m_bDoNotCosim == false) {
			ISLLogError(ERROR_DISCONNECT, "Failed to disconnect from ISL.");
			mexWarnMsgIdAndTxt("ISL:DisconnectFailed", "Failed to disconnect from ISL.");
		}
	}
	else {
		ISLLogInfo(INFO_DISCONNECTED, "Disconnected from the ISL API.");
	}
	if (stInfo->m_cConnect->SendStopSession() == false) {
		ISLLogWarning(WARNING_SENDSTOPEVENT, "Failed to a stop signal to ISL.");
		mexWarnMsgIdAndTxt("ISL:SendStopSessionFailed", "Failed to a stop signal to ISL.");
	}
	//delete stInfo->m_cConnect;
	delete stInfo;
	ssSetUserData(S, NULL);
}

// Is this file being compiled as a MEX-file?
#ifdef	MATLAB_MEX_FILE
// Yes: MEX-file interface mechanism
#include <simulink.c>
#else
// No: Code generation registration function
#include <cg_sfun.h>
#endif // MATLAB_MEX_FILE
