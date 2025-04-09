/*
 *     Name: isl_input.cpp
 *
 *     Description: ISL Input S-Function.
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
#define S_FUNCTION_NAME		isl_input
#define S_FUNCTION_LEVEL	2

// Macros relative to the S-Function interface
// Number of parameters of the S-Function
#define SFUNC_PARAMS_NB 	5


/*
 *     Header files
 */

#include <simstruc.h>
#include <isl_simulink.h>
#include <isl_api.h>
#include "logcodes.h"


/*
 *     Constants definition
 */

enum {
	PARAM_XML_FILE = 0,		// Param[0] = XML file
	PARAM_STORE_OPTION,		// Param[1] = Store option
	PARAM_DONOT_CONNECT,	// Param[2] = Do not connect option
	PARAM_SETSTEP_SIZE,		// Param[3] = Set step size
	PARAM_STEP_SIZE			// Param[4] = Step Size
};


/*
 *     Simulink callback functions definition
 */

static void mdlInitializeSizes(SimStruct * S) {
	// Create a new instance of the structure
	tISLDataInfo * stInfo = new tISLDataInfo;
	stInfo->m_dFixedStep = INHERITED_SAMPLE_TIME;
	stInfo->m_dStep = INHERITED_SAMPLE_TIME;
	stInfo->m_dInitialStep = 1e-6;
	stInfo->m_dLastTime = 0.0;
	stInfo->m_cData = NULL;
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
	// Connector already loaded?
	if (g_cConnect == NULL) {
		// Get the XML file
		size_t nSizeStr = mxGetN(ssGetSFcnParam(S, PARAM_XML_FILE)) * (sizeof(mxChar) + 1);
		if (nSizeStr <= 1) {
			return;
		}
		char * sXMLFile = (char *)mxMalloc(nSizeStr);
		int nStatus = mxGetString(ssGetSFcnParam(S, PARAM_XML_FILE), sXMLFile, (int)nSizeStr);
		if (nStatus != 0) {
			mexErrMsgIdAndTxt("ISL:WrongXMLFile", "Error on the XML file name. Error: %d.", nStatus);
			return;
		}
		// Load the XML file
		g_cConnect = new isl::CConnect();
		g_cConnect->CloseLogOnDelete(true);
		if (g_cConnect->Load(sXMLFile) == false) {
			mxFree(sXMLFile);
			std::string sFile(g_cConnect->GetFileName());
			ISLLogError(ERROR_LOADXML_FAILED, "Failed to load the XML file '%s'.",
				sFile.c_str());
			delete g_cConnect;
			mexErrMsgIdAndTxt("ISL:XMLLoadFailed", "Failed to load the XML file '%s'.",
				sFile.c_str());
			return;
		}
		mxFree(sXMLFile);
		ISLLogInfo(INFO_XML_LOADED, "XML file '%s' loaded.",
			g_cConnect->GetFileName().c_str());
		// Check the number of IOs
		int nNbIOs = g_cConnect->GetNbIOs();
		if (nNbIOs <= 0) {
			ISLLogWarning(WARNING_NOIOSTOCONNECT, "No variable to connect.");
			mexWarnMsgIdAndTxt("ISL:NoVariable", "No variable to connect.");
			g_bDoNotCosim = true;
		}
	}
	// Set the number of inputs of the block
	if (!ssSetNumInputPorts(S, 1)) {
		ISLLogError(ERROR_SETNBOUTPUTS_FAILED, "Failed to set the number of inputs of the block.");
		delete g_cConnect;
		g_cConnect = NULL;
		mexErrMsgIdAndTxt("ISL:WrongInputsNumber", "Failed to set the number of inputs of the block.");
		return;
	}
	ssSetInputPortWidth(S, 0, DYNAMICALLY_SIZED);
	// Set the number of outputs of the block
	if (!ssSetNumOutputPorts(S, 1)) {
		ISLLogError(ERROR_SETNBINPUTS_FAILED, "Failed to set the number of outputs of the block.");
		delete g_cConnect;
		g_cConnect = NULL;
		mexErrMsgIdAndTxt("ISL:WrongOUTPortsNumber", "Failed to set the number of outputs of the block.");
		return;
	}
	ssSetOutputPortWidth(S, 0, DYNAMICALLY_SIZED);
	//
	ssSetNumSampleTimes(S, 1);
	ssSetNumRWork(S, 0);
	ssSetNumIWork(S, 0);
	ssSetNumPWork(S, 0);
	ssSetNumModes(S, 0);
	ssSetNumNonsampledZCs(S, 0);
	// Get the data element from the connector
	std::string sBlockName = GetBlockNameFromPath(ssGetPath(S));
	ISLLogInfo(INFO_IDENTIFY_VARIABLE, "Identifying the variable '%s'...", sBlockName.c_str());
	stInfo->m_cData = g_cConnect->GetIO(sBlockName, NULL);
	if (stInfo->m_cData == NULL) {
		ISLLogError(ERROR_VARIABLE_NOT_DEFINED, "The variable '%s' is not defined on the ISL bus.",
			sBlockName.c_str());
		mexWarnMsgIdAndTxt("ISL:NoVariable", "The variable '%s' is not defined on the ISL bus.",
			sBlockName.c_str());
		g_bDoNotCosim = true;
	}
	//
	ssSetOptions(S, SS_OPTION_EXCEPTION_FREE_CODE
		| SS_OPTION_CALL_TERMINATE_ON_EXIT);
}

static void mdlInitializeSampleTimes(SimStruct * S)
{
	if (g_cConnect == NULL) {
		return;
	}
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLDataInfo * stInfo = (tISLDataInfo *)pData;
	// Do not connect
	int nDoNotConnect = (int )mxGetPr(ssGetSFcnParam(S, PARAM_DONOT_CONNECT))[0];
	if (nDoNotConnect == 1) {
		ISLLogInfo(INFO_DONOTCONNECT_ISL, "Instance not connected to ISL.");
		g_bDoNotCosim = true;
	}
	// Check the simulation status and mode
	SS_SimMode eMode = ssGetSimMode(S);
	if (eMode != SS_SIMMODE_NORMAL) {
		ISLLogInfo(INFO_SIMULATION_MODE, "Simulation mode: %d.", eMode);
		g_bDoNotCosim = true;
	}
#ifndef SS_SIMSTATUS_NOT_DEFINED
	SS_SimStatus eStatus;
	ssGetSimStatus(S, &eStatus);
	if (eStatus != SIMSTATUS_INITIALIZING) {
		ISLLogInfo(INFO_SIMULATION_STATUS, "Simulation status: %d.", eStatus);
		g_bDoNotCosim = true;
	}
#endif // SS_SIMSTATUS_NOT_DEFINED
	// Identify the step size
	stInfo->m_dStep = INHERITED_SAMPLE_TIME;
	if (g_bDoNotCosim == false) {
		// Step size option?
		int nStepSize = (int)mxGetPr(ssGetSFcnParam(S, PARAM_SETSTEP_SIZE))[0];
		if (nStepSize == 1) {
			stInfo->m_dStep = (double)mxGetPr(ssGetSFcnParam(S, PARAM_STEP_SIZE))[0];
			if (stInfo->m_dStep <= 0.0) {
				stInfo->m_dStep = INHERITED_SAMPLE_TIME;
			}
			else {
				stInfo->m_dFixedStep = stInfo->m_dStep;
			}
		}
		// Get step size from the XML file
		double dISLStep = g_cConnect->GetStepSize();
		if (dISLStep > 0.0) {
			stInfo->m_dStep = dISLStep;
			stInfo->m_dFixedStep = stInfo->m_dStep;
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
	if (g_cConnect == NULL) {
		return;
	}
	if (g_bDoNotCosim) {
		return;
	}
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLDataInfo * stInfo = (tISLDataInfo *)pData;
	// Connect if not already connected
	if (g_cConnect->IsConnnected() == false) {
		// Activate the timer
		g_cConnect->StartTimer();
		// Create the session
		if (g_cConnect->Create() == false) {
			ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session '%s'.",
				g_cConnect->GetSessionId().c_str());
			delete g_cConnect;
			g_cConnect = NULL;
			stInfo->m_cData = NULL;
			mexErrMsgIdAndTxt("ISL:FailedCreateSession", "Failed to create the session '%s'.",
				g_cConnect->GetSessionId().c_str());
			return;
		}
		ISLLogInfo(INFO_SESSION_CREATED, "Session '%s' has been created.",
			g_cConnect->GetSessionId().c_str());
		// Listen to a stop signal
		if (g_cConnect->ListenToExitSession() == false) {
			ISLLogWarning(WARNING_LISTENTOEXITSESSION, "Call of ListenToExitSession failed.");
			mexWarnMsgIdAndTxt("ISL:ListenFailed", "Call of ListenToExitSession failed.");
		}
		// Establish the connection
		if (g_cConnect->Connect() == false) {
			ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect to the transmitters on session '%s'.",
				g_cConnect->GetSessionId().c_str());
			delete g_cConnect;
			g_cConnect = NULL;
			stInfo->m_cData = NULL;
			mexErrMsgIdAndTxt("ISL:FailedToConnect", "Failed to connect to the transmitters on session '%s'.",
				g_cConnect->GetSessionId().c_str());
			return;
		}
		ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
		// Store data
		int nStore = (int)mxGetPr(ssGetSFcnParam(S, PARAM_STORE_OPTION))[0];
		if (nStore == 1) {
			mexWarnMsgIdAndTxt("ISL:StoreNotImplemented", "Data storage not yet implemented.");
			//
			// TODO: Implements the ISL store
			//
		}
		ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API: Initialization done.");
	}
	stInfo->m_dLastTime = ssGetTStart(S);
}

static void mdlOutputs(SimStruct * S, int_T nTid)
{
	if (g_cConnect == NULL) {
		return;
	}
	if (g_bDoNotCosim) {
		return;
	}
	void * pData = ssGetUserData(S);
	if (pData == NULL) {
		return;
	}
	tISLDataInfo * stInfo = (tISLDataInfo *)pData;
	if (stInfo->m_cData == NULL) {
		return;
	}
	if (g_cConnect->IsTerminated()) {
		ISLLogInfo(INFO_STOPREQUESTED, "A stop request has been received.");
		ssSetStopRequested(S, 1);
		return;
	}
	// Read the input
	real_T * dVal  = ssGetOutputPortRealSignal(S, 0);
	double dTime = ssGetT(S);
	double dOutTime = ssGetT(S);
#ifdef DEBUG_ISL
	ISLLogDebug(1, DEBUG_INPUT_READ_DATA, "Reading port '%s' at time %gs.",
		stInfo->m_cData->GetName().c_str(), dTime);
#endif // DEBUG_ISL
	bool bRet = stInfo->m_cData->GetData(dVal, &dOutTime, dTime, true);
	bool bStore = (int)mxGetPr(ssGetSFcnParam(S, PARAM_STORE_OPTION))[0] == 1;
	if (bStore || stInfo->m_cData->IsStoreUsed()) {
		stInfo->m_cData->StoreData(dVal, dOutTime);
	}
#ifdef DEBUG_ISL
	ISLLogDebug(1, DEBUG_INPUT_READ_DONE, "Data read (%d):%g at time %gs.", bRet, *dVal, dOutTime);
#endif // DEBUG_ISL
	if (g_cConnect->IsTerminated()) {
		ISLLogInfo(INFO_STOPREQUESTED, "A stop request has been received.");
		ssSetStopRequested(S, 1);
		return;
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
	tISLDataInfo * stInfo = (tISLDataInfo *)pData;
	delete stInfo;
	ssSetUserData(S, NULL);
	if (g_cConnect == NULL) {
		return;
	}
	int nStore = (int)mxGetPr(ssGetSFcnParam(S, PARAM_STORE_OPTION))[0];
	if (nStore == 1) {
		mexWarnMsgIdAndTxt("ISL:StoreNotImplemented", "Data storage not yet implemented.");
		//
		// TODO: Implements store data
		//
	}
	if (g_cConnect->Disconnect() == false) {
		if (g_bDoNotCosim == false) {
			ISLLogError(ERROR_DISCONNECT, "Failed to disconnect from ISL.");
			mexWarnMsgIdAndTxt("ISL:DisconnectFailed", "Failed to disconnect from ISL.");
		}
	}
	else {
		ISLLogInfo(INFO_DISCONNECTED, "Disconnected from the ISL API.");
	}
	if (g_cConnect->SendStopSession() == false) {
		ISLLogWarning(WARNING_SENDSTOPEVENT, "Failed to a stop signal to ISL.");
		mexWarnMsgIdAndTxt("ISL:SendStopSessionFailed", "Failed to a stop signal to ISL.");
	}
	delete g_cConnect;
	//g_cConnect = NULL;
	g_bDoNotCosim = false;
}

// Is this file being compiled as a MEX-file?
#ifdef	MATLAB_MEX_FILE
// Yes: MEX-file interface mechanism
#include <simulink.c>
#else
// No: Code generation registration function
#include <cg_sfun.h>
#endif // MATLAB_MEX_FILE
