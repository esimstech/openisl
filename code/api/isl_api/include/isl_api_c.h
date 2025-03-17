/*
 *     Name: isl_api_c.h
 *
 *     Description: OpenISL C API.
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

#ifndef _ISL_API_C_H_
#define _ISL_API_C_H_

 /*
  *     Header files
  */

#include <isl_api_global.h>
#include <swversion.h>
#include <isl_const.h>


/*
 *     Functions declaration
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	ISL_API_EXPORT const char * ISL_GetVersionNumber();

	ISL_API_EXPORT const char * ISL_GetLogFile();
	ISL_API_EXPORT void ISL_LogError(unsigned int uId, const char * sFormat, ...);
	ISL_API_EXPORT void ISL_LogWarning(unsigned int uId, const char * sFormat, ...);
	ISL_API_EXPORT void ISL_LogInfo(unsigned int uId, const char * sFormat, ...);
	ISL_API_EXPORT void ISL_LogDebug(unsigned int uLevel, unsigned int uId, const char * sFormat, ...);

	ISL_API_EXPORT int ISL_StartThreadListenToStop(void * pConnect);
	ISL_API_EXPORT int ISL_SendStopRequest();
	ISL_API_EXPORT int ISL_SendStopSession(const char * sSession);

	ISL_API_EXPORT void * ISL_ConnectInit(int nIsOwner);
	ISL_API_EXPORT int ISL_ConnectFree(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectNew(void * pConnect, const char * sName);
	ISL_API_EXPORT int ISL_ConnectLoad(void * pConnect, const char * sFileName);
	ISL_API_EXPORT int ISL_ConnectCheck(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSave(void * pConnect, const char * sFileName);

	ISL_API_EXPORT int ISL_ConnectSetName(void * pConnect, const char * sName);
	ISL_API_EXPORT const char * ISL_ConnectGetName(void * pConnect);
	ISL_API_EXPORT const char * ISL_ConnectGetId(void * pConnect);
	ISL_API_EXPORT unsigned int ISL_ConnectGetUId(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetType(void * pConnect, const char * sType);
	ISL_API_EXPORT unsigned int ISL_ConnectGetType(void * pConnect);
	ISL_API_EXPORT const char * ISL_ConnectGetTypeAsStr(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetPID(void * pConnect, unsigned long ulPId);
	ISL_API_EXPORT unsigned long ISL_ConnectGetPID(void * pConnect);
	ISL_API_EXPORT const char * ISL_ConnectGetFileName(void * pConnect);

	ISL_API_EXPORT int ISL_ConnectSetSessionId(void * pConnect, const char * sSessionId);
	ISL_API_EXPORT const char * ISL_ConnectGetSessionId(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetConnectTimeOut(void * pConnect, int nTimeOut);
	ISL_API_EXPORT int ISL_ConnectGetConnectTimeOut(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetStartTime(void * pConnect, double dVal);
	ISL_API_EXPORT double ISL_ConnectGetStartTime(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetEndTime(void * pConnect, double dVal);
	ISL_API_EXPORT double ISL_ConnectGetEndTime(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetStepSize(void * pConnect, double dVal);
	ISL_API_EXPORT double ISL_ConnectGetStepSize(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetStepTolerance(void * pConnect, double dVal);
	ISL_API_EXPORT double ISL_ConnectGetStepTolerance(void * pConnect);

	// Causality values: IN = 0, OUT = 1;
	// Type values: Real = 0, Integer = 1, Boolean = 3, String = 4
	ISL_API_EXPORT void * ISL_ConnectNewIO(void * pConnect, const char * sId, int nCausality, int nType, int nSize);
	ISL_API_EXPORT int ISL_ConnectRemoveIO(void * pConnect, void * pData);

	ISL_API_EXPORT int ISL_ConnectGetNbIOs(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectGetNbInputs(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectGetNbOutputs(void * pConnect);

	ISL_API_EXPORT int ISL_ConnectIOExists(void * pConnect, const char * sId);
	ISL_API_EXPORT void * ISL_ConnectGetIOFromStr(void * pConnect, const char * sId, int * nInd);
	ISL_API_EXPORT void * ISL_ConnectGetIO(void * pConnect, int i);
	ISL_API_EXPORT void * ISL_ConnectGetInputFromStr(void * pConnect, const char * sId);
	ISL_API_EXPORT void * ISL_ConnectGetInput(void * pConnect, int i);
	ISL_API_EXPORT void * ISL_ConnectGetOutputFromStr(void * pConnect, const char * sId);
	ISL_API_EXPORT void * ISL_ConnectGetOutput(void * pConnect, int i);

	ISL_API_EXPORT int ISL_ConnectCloseLogOnDelete(void * pConnect, int nVal);
	ISL_API_EXPORT int ISL_ConnectSetMode(void * pConnect, int nMode);
	ISL_API_EXPORT int ISL_ConnectGetMode(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSetViewer(void * pConnect, int nVal);
	ISL_API_EXPORT int ISL_ConnectIsViewer(void * pConnect);

	ISL_API_EXPORT int ISL_ConnectSetTerminated(void * pConnect, int nVal);
	ISL_API_EXPORT int ISL_ConnectIsTerminated(void * pConnect);

	ISL_API_EXPORT int ISL_ConnectCreate(void * pConnect, const char * sSessionId);
	ISL_API_EXPORT int ISL_ConnectConnect(void * pConnect, int nWait);
	ISL_API_EXPORT int ISL_ConnectDisconnect(void * pConnect);

	ISL_API_EXPORT int ISL_ConnectListenToExitSession(void * pConnect);
	ISL_API_EXPORT int ISL_ConnectSendStopSession(void * pConnect);

	ISL_API_EXPORT const char * ISL_IOGetId(void * pData);
	ISL_API_EXPORT int ISL_IOSetName(void * pData, const char * sName);
	ISL_API_EXPORT const char * ISL_IOGetName(void * pData);
	ISL_API_EXPORT int ISL_IOSetConnectId(void * pData, const char * sId);
	ISL_API_EXPORT const char * ISL_IOGetConnectId(void * pData);

	ISL_API_EXPORT int ISL_IOSetCausality(void * pData, int nCausality);
	ISL_API_EXPORT int ISL_IOGetCausality(void * pData);
	ISL_API_EXPORT int ISL_IOIsOutput(void * pData);
	ISL_API_EXPORT int ISL_IOIsInput(void * pData);

	ISL_API_EXPORT int ISL_IOGetTypeId(void * pData);
	ISL_API_EXPORT void * ISL_IOGetType(void * pData);
	ISL_API_EXPORT int ISL_IOSetType(void * pData, int nType, int nSize);

	ISL_API_EXPORT int ISL_TypeSetInitialAsStr(void * pType, const char * sVal);
	ISL_API_EXPORT int ISL_TypeSetInitial(void * pType, void * pVal);
	ISL_API_EXPORT int ISL_TypeAllocate(void * pType);
	ISL_API_EXPORT int ISL_TypeFree(void * pType);
	ISL_API_EXPORT int ISL_TypeIsAllocated(void * pType);

	ISL_API_EXPORT int ISL_IOSetStepSize(void * pData, double dStep);
	ISL_API_EXPORT int ISL_IOUseStore(void * pData, int nUse);
	ISL_API_EXPORT int ISL_IOIsStoreUsed(void * pData);
	ISL_API_EXPORT int ISL_IOSetSyncTimeout(void * pData, int nTimeout); // Set in seconds
	ISL_API_EXPORT int ISL_IOGetSyncTimeout(void * pData);

	ISL_API_EXPORT int ISL_IOSetData(void * pData, void * pVal, double dTime, int nWait);
	ISL_API_EXPORT int ISL_IOSetDataAndNextStep(void * pData, void * pVal, double dTime, double dStep, int nWait);
	ISL_API_EXPORT int ISL_IOGetLastData(void * pData, void * pVal, double * dTime, int nWait);
	ISL_API_EXPORT int ISL_IOGetData(void * pData, void * pVal, double * dOutTime, double dInTime, int nWait);

	ISL_API_EXPORT int ISL_IOSetEventData(void * pData, void * pVal, int nWait);
	ISL_API_EXPORT int ISL_IOGetEventData(void * pData, void * pVal, int nWait);

	ISL_API_EXPORT int ISL_IOSetDataAt(void * pData, void * pVal, double dTime, int nInd);
	ISL_API_EXPORT int ISL_IOGetDataAt(void * pData, void * pVal, double * dTime, int nInd);

	ISL_API_EXPORT int ISL_IOGetMemData(void * pData, void * pVal, double * dTime, double * dStep, int nInd);
	ISL_API_EXPORT int ISL_IOGetFifoDepth(void * pData);
	ISL_API_EXPORT int ISL_IOGetFifoReaders(void * pData);
	ISL_API_EXPORT int ISL_IOGetFifoIndWriter(void * pData);
	ISL_API_EXPORT int ISL_IOGetFifoIndReader(void * pData, int i);

	ISL_API_EXPORT int ISL_IOStoreData(void * pData, void * pVal, double dTime);

	ISL_API_EXPORT int ISL_SimsGetMaxNb();
	ISL_API_EXPORT int ISL_SimsGet(int nInd);
	ISL_API_EXPORT int ISL_SimsClose();

	ISL_API_EXPORT unsigned int ISL_SimsGetType();
	ISL_API_EXPORT unsigned int ISL_SimsGetId();
	ISL_API_EXPORT unsigned long ISL_SimsGetPID();
	ISL_API_EXPORT int ISL_SimsGetNbIOs();
	ISL_API_EXPORT const char * ISL_SimsGetUUID();
	ISL_API_EXPORT const char * ISL_SimsGetName();
	ISL_API_EXPORT const char * ISL_SimsGetFile();
	ISL_API_EXPORT const char * ISL_SimsGetSessionId();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _ISL_API_C_H_
