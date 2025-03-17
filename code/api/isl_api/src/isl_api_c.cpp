/*
 *     Name: isl_api_c.cpp
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

/*
 *     Header files
 */

#include <map>
#include <string>
#include <cstring>
#include <stdarg.h>
#include <stdarg.h>

#include <isl_log.h>
#include "isl_api.h"
#include "isl_api_c.h"


/*
 *     Macros definition
 */

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define extern
#endif


/*
 *     Functions definition
 */

const char * ISL_AllocateAndCopy(const std::string & sKey, const std::string & sVal)
{
	char * sRes = (char *)ISLInstances->AllocateMemory(sKey, sVal.size() + 1, sizeof(char));
	if (sRes) {
		memcpy(sRes, sVal.c_str(), sVal.size());
		sRes[sVal.size()] = '\0';
	}
	return (const char *)sRes;
}

EXTERN ISL_API_EXPORT const char * ISL_GetVersionNumber()
{
	return ISL_AllocateAndCopy("ISL_GetVersionNumber", isl::CConnect::GetVersionNumber());
}

EXTERN ISL_API_EXPORT const char * ISL_GetLogFile()
{
	return ISL_AllocateAndCopy("ISL_GetLogFile", isl::CUtils::GetLogFile());
}

EXTERN ISL_API_EXPORT void ISL_LogError(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogError(uId, &sVec[0]);
}

EXTERN ISL_API_EXPORT void ISL_LogWarning(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogWarning(uId, &sVec[0]);
}

EXTERN ISL_API_EXPORT void ISL_LogInfo(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogInfo(uId, &sVec[0]);
}

EXTERN ISL_API_EXPORT void ISL_LogDebug(unsigned int uLevel, unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogDebug(uLevel, uId, &sVec[0]);
}

EXTERN ISL_API_EXPORT int ISL_StartThreadListenToStop(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (isl::CConnect::StartThreadListenToStop(cConnect)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_SendStopRequest()
{
	if (isl::CConnect::SendStopRequest()) {
		return 0;
	}
	return -1;
}

EXTERN ISL_API_EXPORT int ISL_SendStopSession(const char * sSession)
{
	if (sSession == 0) {
		return -1;
	}
	if (isl::CConnect::SendStopSession(sSession)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT void * ISL_ConnectInit(int nIsOwner)
{
	isl::CConnect * cConnect = new isl::CConnect(nIsOwner != 0);
	return cConnect;
}

EXTERN ISL_API_EXPORT int ISL_ConnectFree(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	delete cConnect;
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectNew(void * pConnect, const char * sName)
{
	if (pConnect == 0) {
		return -1;
	}
	if (sName == 0) {
		return -2;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->New(sName)) {
		return 0;
	}
	return -3;
}

EXTERN ISL_API_EXPORT int ISL_ConnectLoad(void * pConnect, const char * sFileName)
{
	if (pConnect == 0) {
		return -1;
	}
	if (sFileName == 0) {
		return -2;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->Load(sFileName)) {
		return 0;
	}
	return -3;
}

EXTERN ISL_API_EXPORT int ISL_ConnectCheck(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->Check()) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_ConnectSave(void * pConnect, const char * sFileName)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	std::string sFile;
	if (sFileName != 0) {
		sFile = sFileName;
	}
	if (cConnect->Save(sFile)) {
		return 0;
	}
	return -3;
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetName(void * pConnect, const char * sName)
{
	if (pConnect == 0) {
		return -1;
	}
	if (sName == 0) {
		return -2;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetName(sName);
	return 0;
}

EXTERN ISL_API_EXPORT const char * ISL_ConnectGetName(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return ISL_AllocateAndCopy("ISL_ConnectGetName", cConnect->GetName());
}

EXTERN ISL_API_EXPORT const char * ISL_ConnectGetId(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return ISL_AllocateAndCopy("ISL_ConnectGetId", cConnect->GetId());
}

EXTERN ISL_API_EXPORT unsigned int ISL_ConnectGetUId(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetUId();
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetType(void * pConnect, const char * sType)
{
	if (pConnect == 0) {
		return -1;
	}
	if (sType == 0) {
		return -2;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetType(sType);
	return 0;
}

EXTERN ISL_API_EXPORT unsigned int ISL_ConnectGetType(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetType();
}

EXTERN ISL_API_EXPORT const char * ISL_ConnectGetTypeAsStr(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return ISL_AllocateAndCopy("ISL_ConnectGetTypeAsStr", cConnect->GetTypeAsStr());
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetPID(void * pConnect, unsigned long ulPId)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetPID(ulPId);
	return 0;
}

EXTERN ISL_API_EXPORT unsigned long ISL_ConnectGetPID(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetPID();
}

EXTERN ISL_API_EXPORT const char * ISL_ConnectGetFileName(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	// File name may be empty
	if (cConnect->GetFileName().empty()) {
		return 0;
	}
	return ISL_AllocateAndCopy("ISL_ConnectGetFileName", cConnect->GetFileName());
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetSessionId(void * pConnect, const char * sSessionId)
{
	if (pConnect == 0) {
		return -1;
	}
	if (sSessionId == 0) {
		return -2;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->SetSessionId(sSessionId) == false) {
		return -3;
	}
	return 0;
}

EXTERN ISL_API_EXPORT const char * ISL_ConnectGetSessionId(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->GetSessionId().empty()) {
		return 0;
	}
	return ISL_AllocateAndCopy("ISL_ConnectGetSessionId", cConnect->GetSessionId());
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetConnectTimeOut(void * pConnect, int nTimeOut)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetConnectTimeOut(nTimeOut);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectGetConnectTimeOut(void * pConnect)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetConnectTimeOut();
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetStartTime(void * pConnect, double dVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetStartTime(dVal);
	return 0;
}

EXTERN ISL_API_EXPORT double ISL_ConnectGetStartTime(void * pConnect)
{
	if (pConnect == 0) {
		return 0.0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetStartTime();
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetEndTime(void * pConnect, double dVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetEndTime(dVal);
	return 0;
}

EXTERN ISL_API_EXPORT double ISL_ConnectGetEndTime(void * pConnect)
{
	if (pConnect == 0) {
		return 0.0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetEndTime();
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetStepSize(void * pConnect, double dVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetStepSize(dVal);
	return 0;
}

EXTERN ISL_API_EXPORT double ISL_ConnectGetStepSize(void * pConnect)
{
	if (pConnect == 0) {
		return 0.0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetStepSize();
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetStepTolerance(void * pConnect, double dVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetStepTolerance(dVal);
	return 0;
}

EXTERN ISL_API_EXPORT double ISL_ConnectGetStepTolerance(void * pConnect)
{
	if (pConnect == 0) {
		return 0.0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetStepTolerance();
}

EXTERN ISL_API_EXPORT void * ISL_ConnectNewIO(void * pConnect, const char * sId, int nCausality, int nType, int nSize)
{
	if (pConnect == 0) {
		return 0;
	}
	if (sId == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->NewIO(sId, (isl::CVariable::tCausality )nCausality, (isl::CDataType::tType )nType, nSize);
}

EXTERN ISL_API_EXPORT int ISL_ConnectRemoveIO(void * pConnect, void * pData)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (pData == 0) {
		return -2;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cConnect->RemoveIO(cData) == false) {
		return -3;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectGetNbIOs(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetNbIOs();
}

EXTERN ISL_API_EXPORT int ISL_ConnectGetNbInputs(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetNbInputs();
}

EXTERN ISL_API_EXPORT int ISL_ConnectGetNbOutputs(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetNbOutputs();
}

EXTERN ISL_API_EXPORT int ISL_ConnectIOExists(void * pConnect, const char * sId)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (sId == 0) {
		return -2;
	}
	if (cConnect->IOExists(sId)) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT void * ISL_ConnectGetIOFromStr(void * pConnect, const char * sId, int * nInd)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (sId == 0) {
		return 0;
	}
	return cConnect->GetIO(sId, nInd);
}

EXTERN ISL_API_EXPORT void * ISL_ConnectGetIO(void * pConnect, int i)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetIO(i);
}

EXTERN ISL_API_EXPORT void * ISL_ConnectGetInputFromStr(void * pConnect, const char * sId)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (sId == 0) {
		return 0;
	}
	return cConnect->GetInput(sId);
}

EXTERN ISL_API_EXPORT void * ISL_ConnectGetInput(void * pConnect, int i)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetInput(i);
}

EXTERN ISL_API_EXPORT void * ISL_ConnectGetOutputFromStr(void * pConnect, const char * sId)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (sId == 0) {
		return 0;
	}
	return cConnect->GetOutput(sId);
}

EXTERN ISL_API_EXPORT void * ISL_ConnectGetOutput(void * pConnect, int i)
{
	if (pConnect == 0) {
		return 0;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetOutput(i);
}

EXTERN ISL_API_EXPORT int ISL_ConnectCloseLogOnDelete(void * pConnect, int nVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->CloseLogOnDelete(nVal != 0);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetMode(void * pConnect, int nMode)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetMode((isl::CConnect::tStopMode )nMode);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectGetMode(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	return cConnect->GetMode();
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetViewer(void * pConnect, int nVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetViewer(nVal != 0);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectIsViewer(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->IsViewer()) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectSetTerminated(void * pConnect, int nVal)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	cConnect->SetTerminated(nVal != 0);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectIsTerminated(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->IsTerminated()) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_ConnectCreate(void * pConnect, const char * sSessionId)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	std::string sId;
	if (sSessionId != 0) {
		sId = sSessionId;
	}
	if (cConnect->Create(sId)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_ConnectConnect(void * pConnect, int nWait)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->Connect(nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_ConnectDisconnect(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->Disconnect()) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_ConnectListenToExitSession(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->ListenToExitSession()) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_ConnectSendStopSession(void * pConnect)
{
	if (pConnect == 0) {
		return -1;
	}
	isl::CConnect * cConnect = (isl::CConnect *)pConnect;
	if (cConnect->SendStopSession()) {
		return 0;
	}
	return -1;
}

EXTERN ISL_API_EXPORT const char * ISL_IOGetId(void * pData)
{
	if (pData == 0) {
		return 0;
	}
	isl::CData * cData = (isl::CData *)pData;
	return ISL_AllocateAndCopy("ISL_IOGetId", cData->GetId());
}

EXTERN ISL_API_EXPORT int ISL_IOSetName(void * pData, const char * sName)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (sName == 0) {
		return -2;
	}
	cData->SetName(sName);
	return 0;
}

EXTERN ISL_API_EXPORT const char * ISL_IOGetName(void * pData)
{
	if (pData == 0) {
		return 0;
	}
	isl::CData * cData = (isl::CData *)pData;
	return ISL_AllocateAndCopy("ISL_IOGetName", cData->GetName());
}

EXTERN ISL_API_EXPORT int ISL_IOSetConnectId(void * pData, const char * sId)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (sId == 0) {
		return -2;
	}
	cData->SetConnectId(sId);
	return 0;
}

EXTERN ISL_API_EXPORT const char * ISL_IOGetConnectId(void * pData)
{
	if (pData == 0) {
		return 0;
	}
	isl::CData * cData = (isl::CData *)pData;
	return ISL_AllocateAndCopy("ISL_IOGetConnectId", cData->GetConnectId());
}

EXTERN ISL_API_EXPORT int ISL_IOSetCausality(void * pData, int nCausality)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	cData->SetCausality((isl::CVariable::tCausality)nCausality);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOGetCausality(void * pData)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return cData->GetCausality();
}

EXTERN ISL_API_EXPORT int ISL_IOIsOutput(void * pData)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->IsOutput()) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOIsInput(void * pData)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->IsInput()) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOGetTypeId(void * pData)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return cData->GetTypeId();
}

EXTERN ISL_API_EXPORT void * ISL_IOGetType(void * pData)
{
	if (pData == 0) {
		return 0;
	}
	isl::CData * cData = (isl::CData *)pData;
	return cData->GetType();
}

EXTERN ISL_API_EXPORT int ISL_IOSetType(void * pData, int nType, int nSize)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->SetType((isl::CDataType::tType)nType, nSize)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_TypeSetInitialAsStr(void * pType, const char * sVal)
{
	if (pType == 0) {
		return -1;
	}
	isl::CDataType * cType = (isl::CDataType *)pType;
	if (sVal == 0) {
		return -2;
	}
	if (cType->SetInitial(sVal)) {
		return 0;
	}
	return -3;
}

EXTERN ISL_API_EXPORT int ISL_TypeSetInitial(void * pType, void * pVal)
{
	if (pType == 0) {
		return -1;
	}
	isl::CDataType * cType = (isl::CDataType *)pType;
	if (pVal == 0) {
		return -2;
	}
	if (cType->SetInitial(pVal)) {
		return 0;
	}
	return -3;
}

EXTERN ISL_API_EXPORT int ISL_TypeAllocate(void * pType)
{
	if (pType == 0) {
		return -1;
	}
	isl::CDataType * cType = (isl::CDataType *)pType;
	cType->Allocate();
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_TypeFree(void * pType)
{
	if (pType == 0) {
		return -1;
	}
	isl::CDataType * cType = (isl::CDataType *)pType;
	cType->Free();
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_TypeIsAllocated(void * pType)
{
	if (pType == 0) {
		return -1;
	}
	isl::CDataType * cType = (isl::CDataType *)pType;
	if (cType->IsAllocated()) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOSetStepSize(void * pData, double dStep)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	cData->SetStepSize(dStep);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOUseStore(void * pData, int nUse)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	cData->UseStore(nUse != 0);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOIsStoreUsed(void * pData)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->IsStoreUsed()) {
		return 1;
	}
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOSetSyncTimeout(void * pData, int nTimeout)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	cData->SetSyncTimeout(nTimeout);
	return 0;
}

EXTERN ISL_API_EXPORT int ISL_IOGetSyncTimeout(void * pData)
{
	if (pData == 0) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return cData->GetSyncTimeout();
}

EXTERN ISL_API_EXPORT int ISL_IOSetData(void * pData, void * pVal, double dTime, int nWait)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->SetData(pVal, dTime, nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOSetDataAndNextStep(void * pData, void * pVal, double dTime, double dStep, int nWait)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->SetData(pVal, dTime, dStep, nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOGetLastData(void * pData, void * pVal, double * dTime, int nWait)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->GetData(pVal, dTime, nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOGetData(void * pData, void * pVal, double * dOutTime, double dInTime, int nWait)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->GetData(pVal, dOutTime, dInTime, nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOSetEventData(void * pData, void * pVal, int nWait)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->SetEventData(pVal, nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOGetEventData(void * pData, void * pVal, int nWait)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->GetEventData(pVal, nWait != 0)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOSetDataAt(void * pData, void * pVal, double dTime, int nInd)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->SetData(pVal, dTime, nInd)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOGetDataAt(void * pData, void * pVal, double * dTime, int nInd)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->GetData(pVal, dTime, nInd)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOGetMemData(void * pData, void * pVal, double * dTime, double * dStep, int nInd)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->GetMemData(pVal, dTime, dStep, nInd)) {
		return 0;
	}
	return -2;
}

EXTERN ISL_API_EXPORT int ISL_IOGetFifoDepth(void * pData)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return (int )(cData->GetFifoDepth());
}

EXTERN ISL_API_EXPORT int ISL_IOGetFifoReaders(void * pData)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return cData->GetReaders();
}

EXTERN ISL_API_EXPORT int ISL_IOGetFifoIndWriter(void * pData)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return (int)(cData->GetIndWriter());
}

EXTERN ISL_API_EXPORT int ISL_IOGetFifoIndReader(void * pData, int i)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	return (int)(cData->GetIndReader(i));
}

EXTERN ISL_API_EXPORT int ISL_IOStoreData(void * pData, void * pVal, double dTime)
{
	if (pData == NULL) {
		return -1;
	}
	isl::CData * cData = (isl::CData *)pData;
	if (cData->IsStoreUsed() == false) {
		return -2;
	}
	if (cData->StoreData(pVal, dTime)) {
		return 0;
	}
	return -3;
}

EXTERN ISL_API_EXPORT int ISL_SimsGetMaxNb()
{
	return ISLSims->GetMaxNb();
}

EXTERN ISL_API_EXPORT int ISL_SimsGet(int nInd)
{
	if (ISLSims->Get(nInd)) {
		return 0;
	}
	return -1;
}

EXTERN ISL_API_EXPORT int ISL_SimsClose()
{
	ISLSims_Close;
	return 0;
}

EXTERN ISL_API_EXPORT unsigned int ISL_SimsGetType()
{
	return ISLSims->GetType();
}

EXTERN ISL_API_EXPORT unsigned int ISL_SimsGetId()
{
	return ISLSims->GetId();
}

EXTERN ISL_API_EXPORT unsigned long ISL_SimsGetPID()
{
	return ISLSims->GetPID();
}

EXTERN ISL_API_EXPORT int ISL_SimsGetNbIOs()
{
	return ISLSims->GetNbIOs();
}

EXTERN ISL_API_EXPORT const char * ISL_SimsGetUUID()
{
	return ISL_AllocateAndCopy("ISL_SimsGetUUID", ISLSims->GetUUID());
}

EXTERN ISL_API_EXPORT const char * ISL_SimsGetName()
{
	return ISL_AllocateAndCopy("ISL_SimsGetName", ISLSims->GetName());
}

EXTERN ISL_API_EXPORT const char * ISL_SimsGetFile()
{
	return ISL_AllocateAndCopy("ISL_SimsGetFile", ISLSims->GetFile());
}

EXTERN ISL_API_EXPORT const char * ISL_SimsGetSessionId()
{
	return ISL_AllocateAndCopy("ISL_SimsGetSessionId", ISLSims->GetSessionId());
}
