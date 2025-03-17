/* 
 *     Name: isl_java.cpp
 *
 *     Description: Java coupling for OpenISL.
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
#include "isl_java.h"
#ifndef WIN32
#include <cstring>
#define strcpy_s(dest, size, source)  std::strncpy(dest, source, size);
#endif


/*
 *     Global variables definition
 */

static isl::CConnect * g_cConnect = 0;


/*
 *     Global functions definition
 */

 /*
  * Class:     com_openisl_java_CISLConnect
  * Method:    Initialize
  * Signature: (I)I
  */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Initialize
	(JNIEnv * jEnv, jobject jObj, jint nOwner)
{
	g_cConnect = new isl::CConnect(nOwner != 0);
	g_cConnect->CloseLogOnDelete(true);
	isl::CUtils::Info(99998, "DLL version: %s", GET_APP_VERSION(VERSION_NUMBER));
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    New
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_New
	(JNIEnv * jEnv, jobject jObj, jstring sName)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->New(jEnv->GetStringUTFChars(sName, 0))) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Load
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Load
	(JNIEnv * jEnv, jobject jObj, jstring sFileName)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->Load(jEnv->GetStringUTFChars(sFileName, 0))) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Save
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Save
	(JNIEnv * jEnv, jobject jObj, jstring sFileName)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->Save(jEnv->GetStringUTFChars(sFileName, 0))) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetViewer
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetViewer
	(JNIEnv * jEnv, jobject jObj, jint nMode)
{
	if (g_cConnect == 0) {
		return -1;
	}
	g_cConnect->SetViewer(nMode != 0);
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetSessionId
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetSessionId
	(JNIEnv * jEnv, jobject jObj, jstring sSessionId)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->SetSessionId(jEnv->GetStringUTFChars(sSessionId, 0))) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Create
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Create
	(JNIEnv * jEnv, jobject jObj, jstring sSessionId)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->Create(jEnv->GetStringUTFChars(sSessionId, 0))) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Check
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Check
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->Check()) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Connect
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Connect
	(JNIEnv * jEnv, jobject jObj, jint nWait)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->Connect(nWait != 0)) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    IsTerminated
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_IsTerminated
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == NULL) {
		return -1;
	}
	if (g_cConnect->IsTerminated()) {
		return 0;
	}
	return 1;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Disconnect
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_Disconnect
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->Disconnect()) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    Free
 * Signature: ()V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLConnect_Free
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect != 0) {
		delete g_cConnect;
	}
	g_cConnect = 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetType
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetType
	(JNIEnv * jEnv, jobject jObj, jstring sType)
{
	if (g_cConnect == 0) {
		return -1;
	}
	g_cConnect->SetType(jEnv->GetStringUTFChars(sType, 0));
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    OpenStore
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_OpenStore
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	// TODO: To implement
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    CloseStore
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_CloseStore
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	// TODO: To implement
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetMode
 * Signature: (I)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetMode
	(JNIEnv * jEnv, jobject jObj, jint nMode)
{
	if (g_cConnect == 0) {
		return -1;
	}
	g_cConnect->SetMode((isl::CConnect::tStopMode )nMode);
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    StartListenToExit
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_StartListenToExit
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (isl::CConnect::StartThreadListenToStop(g_cConnect)) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    ListenToExitSession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_ListenToExitSession
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->ListenToExitSession()) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SendStopRequest
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SendStopRequest
	(JNIEnv * jEnv, jobject jObj)
{
	if (isl::CConnect::SendStopRequest()) {
		return 0;
	}
	return -1;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SendStopSession
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SendStopSession
	(JNIEnv * jEnv, jobject jObj, jstring sSessionId)
{
	if (isl::CConnect::SendStopSession(jEnv->GetStringUTFChars(sSessionId, 0))) {
		return 0;
	}
	return -1;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SendStopMySession
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SendStopMySession
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	if (g_cConnect->SendStopSession()) {
		return 0;
	}
	return -2;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetStartTime
 * Signature: (D)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetStartTime
	(JNIEnv * jEnv, jobject jObj, jdouble dVal)
{
	if (g_cConnect == 0) {
		return -1;
	}
	g_cConnect->SetStartTime(dVal);
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    GetStartTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_openisl_java_CISLConnect_GetStartTime
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1.0;
	}
	return g_cConnect->GetStartTime();
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetEndTime
 * Signature: (D)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetEndTime
	(JNIEnv * jEnv, jobject jObj, jdouble dVal)
{
	if (g_cConnect == 0) {
		return -1;
	}
	g_cConnect->SetEndTime(dVal);
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    GetEndTime
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_openisl_java_CISLConnect_GetEndTime
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1.0;
	}
	return g_cConnect->GetEndTime();
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    SetStepSize
 * Signature: (D)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_SetStepSize
	(JNIEnv * jEnv, jobject jObj, jdouble dVal)
{
	if (g_cConnect == 0) {
		return -1;
	}
	g_cConnect->SetStepSize(dVal);
	return 0;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    GetStepSize
 * Signature: ()D
 */
JNIEXPORT jdouble JNICALL Java_com_openisl_java_CISLConnect_GetStepSize
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return 0.0;
	}
	return g_cConnect->GetStepSize();
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    GetNbIOs
 * Signature: ()I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_GetNbIOs
	(JNIEnv * jEnv, jobject jObj)
{
	if (g_cConnect == 0) {
		return -1;
	}
	return g_cConnect->GetNbIOs();
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    GetIOFromId
 * Signature: (Ljava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_GetIOFromId
	(JNIEnv * jEnv, jobject jObj, jstring sId)
{
	if (g_cConnect == 0) {
		return -1;
	}
	int nInd = -3;
	if (g_cConnect->GetIO(jEnv->GetStringUTFChars(sId, 0), &nInd) == 0) {
		return -2;
	}
	return nInd;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    NewIO
 * Signature: (Ljava/lang/String;III)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLConnect_NewIO
	(JNIEnv * jEnv, jobject jObj, jstring sId, jint nCausality, jint nType, jint nSize)
{
	if (g_cConnect == 0) {
		return -1;
	}
	isl::CData * cData = g_cConnect->NewIO(jEnv->GetStringUTFChars(sId, 0),
		(isl::CVariable::tCausality)nCausality, (isl::CDataType::tType)nType, nSize);
	if (cData == 0) {
		return -2;
	}
	int nInd = -4;
	if (g_cConnect->GetIO(jEnv->GetStringUTFChars(sId, 0), &nInd) == 0) {
		return -3;
	}
	return nInd;
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    PrintError
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLConnect_PrintError
	(JNIEnv * jEnv, jobject jObj, jint nId, jstring sMsg)
{
	isl::CUtils::Error(nId, jEnv->GetStringUTFChars(sMsg, 0));
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    PrintWarning
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLConnect_PrintWarning
	(JNIEnv * jEnv, jobject jObj, jint nId, jstring sMsg)
{
	isl::CUtils::Warning(nId, jEnv->GetStringUTFChars(sMsg, 0));
}

/*
 * Class:     com_openisl_java_CISLConnect
 * Method:    PrintInfo
 * Signature: (ILjava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLConnect_PrintInfo
	(JNIEnv * jEnv, jobject jObj, jint nId, jstring sMsg)
{
	isl::CUtils::Info(nId, jEnv->GetStringUTFChars(sMsg, 0));
}

/* Header for class com_openisl_java_CISLData */

/*
 * Class:     com_openisl_java_CISLData
 * Method:    SetConnectionId
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLData_SetConnectionId
	(JNIEnv * jEnv, jobject jObj, jint nInd, jstring sId)
{
	if (g_cConnect == 0) {
		return -1;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return -2;
	}
	cData->SetConnectId(jEnv->GetStringUTFChars(sId, 0));
	return 0;
}


/* Header for class com_openisl_java_CISLDoubleData */

/*
 * Class:     com_openisl_java_CISLDoubleData
 * Method:    GetDouble
 * Signature: (I[DI)D
 */
JNIEXPORT jdouble JNICALL Java_com_openisl_java_CISLDoubleData_GetDouble
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdoubleArray dJTimes, jint nWait)
{
	if (g_cConnect == 0) {
		return 0.0;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return 0.0;
	}
	jsize nLen = jEnv->GetArrayLength(dJTimes);
	if (nLen != 2) {
		return 0.0;
	}
	jboolean bIsCopy = JNI_FALSE;
	jdouble * dTimes = jEnv->GetDoubleArrayElements(dJTimes, &bIsCopy);
	double dVal = 0.0;
	double dOutTime = dTimes[0];
	if (cData->GetData(&dVal, &dOutTime, dTimes[1], nWait != 0)) {
		dTimes[0] = dOutTime;
	}
	jEnv->ReleaseDoubleArrayElements(dJTimes, dTimes, 0);
	return dVal;
}

/*
 * Class:     com_openisl_java_CISLDoubleData
 * Method:    SetDouble
 * Signature: (IDDI)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLDoubleData_SetDouble
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdouble dVal, jdouble dTime, jint nWait)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	cData->SetData(&dVal, dTime, nWait != 0);
}

/*
 * Class:     com_openisl_java_CISLDoubleData
 * Method:    SetDoubleAndStep
 * Signature: (IDDDI)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLDoubleData_SetDoubleAndStep
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdouble dVal, jdouble dTime, jdouble dStep, jint nWait)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	cData->SetData(&dVal, dTime, dStep, nWait != 0);
}

/*
 * Class:     com_openisl_java_CISLDoubleData
 * Method:    StoreDouble
 * Signature: (IDD)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLDoubleData_StoreDouble
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdouble dVal, jdouble dTime)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	cData->StoreData(&dVal, dTime);
}

/*
 * Class:     com_openisl_java_CISLDoubleData
 * Method:    InitDouble
 * Signature: (ID)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLDoubleData_InitDouble
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdouble dVal)
{
	if (g_cConnect == 0) {
		return -1;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return -2;
	}
	if (cData->GetType() == 0) {
		return -3;
	}
	cData->GetType()->Allocate();
	if (cData->GetType()->SetInitial(&dVal)) {
		return 0;
	}
	return -4;
}


/* Header for class com_openisl_java_CISLIntData */

/*
 * Class:     com_openisl_java_CISLIntData
 * Method:    GetInt
 * Signature: (I[DI)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLIntData_GetInt
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdoubleArray dJTimes, jint nWait)
{
	if (g_cConnect == 0) {
		return 0;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return 0;
	}
	jsize nLen = jEnv->GetArrayLength(dJTimes);
	if (nLen != 2) {
		return 0;
	}
	jboolean bIsCopy = JNI_FALSE;
	jdouble * dTimes = jEnv->GetDoubleArrayElements(dJTimes, &bIsCopy);
	int nVal = 0;
	double dOutTime = dTimes[0];
	if (cData->GetData(&nVal, &dOutTime, dTimes[1], nWait != 0)) {
		dTimes[0] = dOutTime;
	}
	jEnv->ReleaseDoubleArrayElements(dJTimes, dTimes, 0);
	return nVal;
}

/*
 * Class:     com_openisl_java_CISLIntData
 * Method:    SetInt
 * Signature: (IIDI)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLIntData_SetInt
	(JNIEnv * jEnv, jobject jObj, jint nInd, jint nVal, jdouble dTime, jint nWait)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	cData->SetData(&nVal, dTime, nWait != 0);
}

/*
 * Class:     com_openisl_java_CISLIntData
 * Method:    SetIntAndStep
 * Signature: (IIDDI)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLIntData_SetIntAndStep
	(JNIEnv * jEnv, jobject jObj, jint nInd, jint nVal, jdouble dTime, jdouble dStep, jint nWait)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	cData->SetData(&nVal, dTime, dStep, nWait != 0);
}

/*
 * Class:     com_openisl_java_CISLIntData
 * Method:    StoreInt
 * Signature: (IID)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLIntData_StoreInt
	(JNIEnv * jEnv, jobject jObj, jint nInd, jint nVal, jdouble dTime)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	cData->StoreData(&nVal, dTime);
}

/*
 * Class:     com_openisl_java_CISLIntData
 * Method:    InitInteger
 * Signature: (II)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLIntData_InitInteger
	(JNIEnv * jEnv, jobject jObj, jint nInd, jint nVal)
{
	if (g_cConnect == 0) {
		return -1;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return -2;
	}
	if (cData->GetType() == 0) {
		return -3;
	}
	cData->GetType()->Allocate();
	if (cData->GetType()->SetInitial(&nVal)) {
		return 0;
	}
	return -4;
}


/* Header for class com_openisl_java_CISLStringData */

/*
 * Class:     com_openisl_java_CISLStringData
 * Method:    GetString
 * Signature: (I[DI)Ljava/lang/String;
 */
JNIEXPORT jstring JNICALL Java_com_openisl_java_CISLStringData_GetString
	(JNIEnv * jEnv, jobject jObj, jint nInd, jdoubleArray dJTimes, jint nWait)
{
	if (g_cConnect == 0) {
		return jEnv->NewStringUTF("");
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return jEnv->NewStringUTF("");
	}
	jsize nLen = jEnv->GetArrayLength(dJTimes);
	if (nLen != 2) {
		return jEnv->NewStringUTF("");
	}
	bool bOk = false;
	jboolean bIsCopy = JNI_FALSE;
	jdouble * dTimes = jEnv->GetDoubleArrayElements(dJTimes, &bIsCopy);
	char * sVal = (char *)calloc(cData->GetType()->GetSize(), sizeof(char));
	double dOutTime = dTimes[0];
	if (cData->GetData(sVal, &dOutTime, dTimes[1], nWait != 0)) {
		bOk = true;
		dTimes[0] = dOutTime;
	}
	jEnv->ReleaseDoubleArrayElements(dJTimes, dTimes, 0);
	jstring sRes = 0;
	if (bOk) {
		sRes = jEnv->NewStringUTF((const char *)sVal);
	}
	else  {
		sRes = jEnv->NewStringUTF("");
	}
	free(sVal);
	return sRes;
}

/*
 * Class:     com_openisl_java_CISLStringData
 * Method:    SetString
 * Signature: (ILjava/lang/String;DI)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLStringData_SetString
	(JNIEnv * jEnv, jobject jObj, jint nInd, jstring sJVal, jdouble dTime, jint nWait)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	int nSize = cData->GetType()->GetSize();
	char * sVal = (char *)calloc(nSize, sizeof(char));
	strcpy_s(sVal, nSize, jEnv->GetStringUTFChars(sJVal, 0));
	cData->SetData(sVal, dTime, nWait != 0);
	free(sVal);
}

/*
 * Class:     com_openisl_java_CISLStringData
 * Method:    SetStringAndStep
 * Signature: (ILjava/lang/String;DDI)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLStringData_SetStringAndStep
	(JNIEnv * jEnv, jobject jObj, jint nInd, jstring sJVal, jdouble dTime, jdouble dStep, jint nWait)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	int nSize = cData->GetType()->GetSize();
	char * sVal = (char *)calloc(nSize, sizeof(char));
	strcpy_s(sVal, nSize, jEnv->GetStringUTFChars(sJVal, 0));
	cData->SetData(sVal, dTime, dStep, nWait != 0);
	free(sVal);
}

/*
 * Class:     com_openisl_java_CISLStringData
 * Method:    StoreString
 * Signature: (ILjava/lang/String;D)V
 */
JNIEXPORT void JNICALL Java_com_openisl_java_CISLStringData_StoreString
	(JNIEnv * jEnv, jobject jObj, jint nInd, jstring sJVal, jdouble dTime)
{
	if (g_cConnect == 0) {
		return;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return;
	}
	int nSize = cData->GetType()->GetSize();
	char * sVal = (char *)calloc(nSize, sizeof(char));
	strcpy_s(sVal, nSize, jEnv->GetStringUTFChars(sJVal, 0));
	cData->StoreData(sVal, dTime);
	free(sVal);
}

/*
 * Class:     com_openisl_java_CISLStringData
 * Method:    InitString
 * Signature: (ILjava/lang/String;)I
 */
JNIEXPORT jint JNICALL Java_com_openisl_java_CISLStringData_InitString
	(JNIEnv * jEnv, jobject jObj, jint nInd, jstring sJVal)
{
	if (g_cConnect == 0) {
		return -1;
	}
	isl::CData * cData = g_cConnect->GetIO(nInd);
	if (cData == 0) {
		return -2;
	}
	if (cData->GetType() == 0) {
		return -3;
	}
	int nSize = cData->GetType()->GetSize();
	char * sVal = (char *)calloc(nSize, sizeof(char));
	strcpy_s(sVal, nSize, jEnv->GetStringUTFChars(sJVal, 0));
	cData->GetType()->Allocate();
	int nRes = -4;
	if (cData->GetType()->SetInitial(sVal)) {
		nRes = 0;
	}
	free(sVal);
	return nRes;
}
