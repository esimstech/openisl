//
//     Name: isl_cli.cpp
//
//     Description: OpenISL .Net API.
//
//     Author: T. Roudier
//     Copyright (c) 2019-2025 E-Sim Solutions Inc
//
//    Distributed under the MIT License.
// 
//     --------------------------------------------------------------------------
// 
//     Permission is hereby granted, free of charge, to any person obtaining a
//     copy of this software and associated documentation files (the “Software”),
//     to deal in the Software without restriction, including without limitation
//     the rights to use, copy, modify, merge, publish, distribute, sublicense,
//     and/or sell copies of the Software, and to permit persons to whom the
//     Software is furnished to do so, subject to the following conditions:
//
//     The above copyright notice and this permission notice shall be included in
//     all copies or substantial portions of the Software.
//
//     THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
//     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
//     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
//     DEALINGS IN THE SOFTWARE.
// 
//     --------------------------------------------------------------------------
//
//

//
//     Header files
//

#include <msclr/marshal_cppstd.h>

#include <isl_api.h>
#include "isl_cli.h"


//
//     Using namespaces
//

using namespace System;
using namespace System::Runtime::InteropServices;
using namespace isl_cli;


//
//     Classes Definition
//

//
//     Class CData
//

CData::CData(isl::CData * cData)
{
	m_cData = cData;
	m_eType = tDataType::DATATYPE_NONE;
	if (cData != 0) {
		m_eType = (tDataType)(cData->GetTypeId());
		m_eCausality = (tCausality)(cData->GetCausality());
	}
}

CData::~CData()
{
	m_cData = 0;
}

String^ CData::GetId()
{
	if (m_cData != 0) {
		String^ sId = gcnew String(m_cData->GetId().c_str());
		return sId;
	}
	return nullptr;
}

void CData::SetName(String ^ sName)
{
	if (m_cData != 0) {
		m_cData->SetName(msclr::interop::marshal_as< std::string >(sName));
	}
}

String^ CData::GetName()
{
	if (m_cData != 0) {
		String^ sName = gcnew String(m_cData->GetName().c_str());
		return sName;
	}
	return nullptr;
}

void CData::SetConnectId(String ^ sId)
{
	if (m_cData != 0) {
		m_cData->SetConnectId(msclr::interop::marshal_as< std::string >(sId));
	}
}

String^ CData::GetConnectId()
{
	if (m_cData != 0) {
		String^ sId = gcnew String(m_cData->GetConnectId().c_str());
		return sId;
	}
	return nullptr;
}

CData::tDataType CData::GetType()
{
	return m_eType;
}

Int32 CData::GetSize()
{
	if (m_cData != NULL) {
		if (m_cData->GetType() != NULL) {
			return m_cData->GetType()->GetSize();
		}
	}
	return 1;
}

Boolean CData::IsInput()
{
	if (m_cData != NULL) {
		return m_cData->IsInput();
	}
	return false;
}

Boolean CData::IsOutput()
{
	if (m_cData != NULL) {
			return m_cData->IsOutput();
	}
	return false;
}

void CData::SetFifoDepth(Int16 nDepth)
{
	if (m_cData != NULL) {
		m_cData->SetFifoDepth(nDepth);
	}
}

Int16 CData::GetFifoDepth()
{
	if (m_cData != NULL) {
		return m_cData->GetFifoDepth();
	}
	return 0;
}

Int16 CData::GetMaxFifoDepth()
{
	if (m_cData != NULL) {
		return m_cData->GetMaxFifoDepth();
	}
	return 0;
}

Int32 CData::GetMaxReaders()
{
	if (m_cData != NULL) {
		return m_cData->GetMaxReaders();
	}
	return 0;
}

void CData::Allocate()
{
	if (m_cData != 0) {
		if (m_cData->GetType() != 0) {
			m_cData->GetType()->Allocate();
		}
	}
}

Boolean CData::SetInitial(Object ^ pData)
{
	if (m_cData == 0) {
		return false;
	}
	if (m_cData->GetType() == 0) {
		return false;
	}
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			bool bVal = *safe_cast<Boolean^>(pData);
			return m_cData->GetType()->SetInitial(&bVal);
		}
		case tDataType::DATATYPE_INTEGER:
		{
			int nVal = *safe_cast<Int32^>(pData);
			return m_cData->GetType()->SetInitial(&nVal);
		}
		case tDataType::DATATYPE_REAL:
		{
			double dVal = *safe_cast<Double^>(pData);
			return m_cData->GetType()->SetInitial(&dVal);
		}
		case tDataType::DATATYPE_STRING:
		{
			String^ sTmpVal = safe_cast<String^>(pData);
			char* sVal = (char*)(void*)Marshal::StringToHGlobalAnsi(sTmpVal);
			return m_cData->GetType()->SetInitial(&sVal);
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return false;
}

Boolean CData::Initialize(Double dTime)
{
	if (m_cData != NULL) {
		return m_cData->Initialize(dTime);
	}
	return false;
}

Boolean CData::SetData(Object^ pData, Double dTime, Boolean bWait)
{
	if (m_cData == NULL) {
		return false;
	}
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			bool bVal = *safe_cast<Boolean^>(pData);
			return m_cData->SetData(&bVal, dTime, bWait);
		}
		case tDataType::DATATYPE_INTEGER:
		{
			int nVal = *safe_cast<Int32^>(pData);
			return m_cData->SetData(&nVal, dTime, bWait);
		}
		case tDataType::DATATYPE_REAL:
		{
			double dVal = *safe_cast<Double^>(pData);
			return m_cData->SetData(&dVal, dTime, bWait);
		}
		case tDataType::DATATYPE_STRING:
		{
			String^ sTmpVal = safe_cast<String^>(pData);
			char* sVal = (char*)(void*)Marshal::StringToHGlobalAnsi(sTmpVal);
			return m_cData->SetData(&sVal, dTime, bWait);
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return false;
}

Boolean CData::SetData(Object^ pData, Double dTime, Double dStep, Boolean bWait)
{
	if (m_cData == NULL) {
		return false;
	}
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			bool bVal = *safe_cast<Boolean^>(pData);
			return m_cData->SetData(&bVal, dTime, dStep, bWait);
		}
		case tDataType::DATATYPE_INTEGER:
		{
			int nVal = *safe_cast<Int32^>(pData);
			return m_cData->SetData(&nVal, dTime, dStep, bWait);
		}
		case tDataType::DATATYPE_REAL:
		{
			double dVal = *safe_cast<Double^>(pData);
			return m_cData->SetData(&dVal, dTime, dStep, bWait);
		}
		case tDataType::DATATYPE_STRING:
		{
			String^ sTmpVal = safe_cast<String^>(pData);
			char* sVal = (char*)(void*)Marshal::StringToHGlobalAnsi(sTmpVal);
			return m_cData->SetData(&sVal, dTime, dStep, bWait);
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return false;
}

Boolean CData::GetData(Object^% pData, Double % dTime, Boolean bWait)
{
	if (m_cData == NULL) {
		return false;
	}
	Boolean bRet = false;
	double dRetTime = dTime;
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			Boolean^ bOut = safe_cast<Boolean^>(pData);
			bool bVal = false;
			bRet = m_cData->GetData(&bVal, &dRetTime, bWait);
			dTime = dRetTime;
			*bOut = bVal;
			break;
		}
		case tDataType::DATATYPE_INTEGER:
		{
			Int32^ nOut = safe_cast<Int32^>(pData);
			int nVal = 0;
			bRet = m_cData->GetData(&nVal, &dRetTime, bWait);
			dTime = dRetTime;
			*nOut = nVal;
			break;
		}
		case tDataType::DATATYPE_REAL:
		{
			Double^ dOut = safe_cast<Double^>(pData);
			double dVal = 0;
			bRet = m_cData->GetData(&dVal, &dRetTime, bWait);
			dTime = dRetTime;
			*dOut = dVal;
			break;
		}
		case tDataType::DATATYPE_STRING:
		{
			char * sVal = (char *)calloc(GetSize(), sizeof(char));
			bRet = m_cData->GetData(&sVal, &dRetTime, bWait);
			dTime = dRetTime;
			pData = gcnew String(sVal);
			free(sVal);
			break;
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return bRet;
}

Boolean CData::GetData(Object^% pData, Double % dOutTime, Double dInTime, Boolean bWait)
{
	if (m_cData == NULL) {
		return false;
	}
	Boolean bRet = false;
	double dRetTime = dOutTime;
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			Boolean^ bOut = safe_cast<Boolean^>(pData);
			bool bVal = false;
			bRet = m_cData->GetData(&bVal, &dRetTime, dInTime, bWait);
			dOutTime = dRetTime;
			*bOut = bVal;
			break;
		}
		case tDataType::DATATYPE_INTEGER:
		{
			Int32^ nOut = safe_cast<Int32^>(pData);
			int nVal = 0;
			bRet = m_cData->GetData(&nVal, &dRetTime, dInTime, bWait);
			dOutTime = dRetTime;
			*nOut = nVal;
			break;
		}
		case tDataType::DATATYPE_REAL:
		{
			Double^ dOut = safe_cast<Double^>(pData);
			double dVal = 0;
			bRet = m_cData->GetData(&dVal, &dRetTime, dInTime, bWait);
			dOutTime = dRetTime;
			*dOut = dVal;
			break;
		}
		case tDataType::DATATYPE_STRING:
		{
			char * sVal = (char *)calloc(GetSize(), sizeof(char));
			bRet = m_cData->GetData(&sVal, &dRetTime, dInTime, bWait);
			dOutTime = dRetTime;
			pData = gcnew String(sVal);
			free(sVal);
			break;
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return bRet;
}

Boolean CData::SetData(Object^ pData, Double dTime, Int32 nInd)
{
	if (m_cData == NULL) {
		return false;
	}
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			bool bVal = *safe_cast<Boolean^>(pData);
			return m_cData->SetData(&bVal, dTime, nInd);
		}
		case tDataType::DATATYPE_INTEGER:
		{
			int nVal = *safe_cast<Int32^>(pData);
			return m_cData->SetData(&nVal, dTime, nInd);
		}
		case tDataType::DATATYPE_REAL:
		{
			double dVal = *safe_cast<Double^>(pData);
			return m_cData->SetData(&dVal, dTime, nInd);
		}
		case tDataType::DATATYPE_STRING:
		{
			String^ sTmpVal = safe_cast<String^>(pData);
			char* sVal = (char*)(void*)Marshal::StringToHGlobalAnsi(sTmpVal);
			return m_cData->SetData(&sVal, dTime, nInd);
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return false;
}

Boolean CData::GetData(Object^% pData, Double % dTime, Int32 nInd)
{
	if (m_cData == NULL) {
		return false;
	}
	Boolean bRet = false;
	double dRetTime = dTime;
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			Boolean^ bOut = safe_cast<Boolean^>(pData);
			bool bVal = false;
			bRet = m_cData->GetData(&bVal, &dRetTime, nInd);
			dTime = dRetTime;
			*bOut = bVal;
			break;
		}
		case tDataType::DATATYPE_INTEGER:
		{
			Int32^ nOut = safe_cast<Int32^>(pData);
			int nVal = 0;
			bRet = m_cData->GetData(&nVal, &dRetTime, nInd);
			dTime = dRetTime;
			*nOut = nVal;
			break;
		}
		case tDataType::DATATYPE_REAL:
		{
			Double^ dOut = safe_cast<Double^>(pData);
			double dVal = 0;
			bRet = m_cData->GetData(&dVal, &dRetTime, nInd);
			dTime = dRetTime;
			*dOut = dVal;
			break;
		}
		case tDataType::DATATYPE_STRING:
		{
			char * sVal = (char *)calloc(GetSize(), sizeof(char));
			bRet = m_cData->GetData(&sVal, &dRetTime, nInd);
			dTime = dRetTime;
			pData = gcnew String(sVal);
			free(sVal);
			break;
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return bRet;
}

Boolean CData::StoreData(Object^ pData, Double dTime)
{
	if (m_cData == NULL) {
		return false;
	}
	if (m_cData->IsStoreUsed() == false) {
		return true; // No need to store
	}
	switch (m_eType) {
		case tDataType::DATATYPE_BOOLEAN:
		{
			bool bVal = *safe_cast<Boolean^>(pData);
			return m_cData->StoreData(&bVal, dTime);
		}
		case tDataType::DATATYPE_INTEGER:
		{
			int nVal = *safe_cast<Int32^>(pData);
			return m_cData->StoreData(&nVal, dTime);
		}
		case tDataType::DATATYPE_REAL:
		{
			double dVal = *safe_cast<Double^>(pData);
			return m_cData->StoreData(&dVal, dTime);
		}
		case tDataType::DATATYPE_STRING:
		{
			String^ sTmpVal = safe_cast<String^>(pData);
			char* sVal = (char*)(void*)Marshal::StringToHGlobalAnsi(sTmpVal);
			return m_cData->StoreData(&sVal, dTime);
		}
		case tDataType::DATATYPE_NONE:
		default:
			break;
	}
	return false;
}

Double CData::GetOriginalStep()
{
	if (m_cData == NULL) {
		return 0.0;
	}
	return m_cData->GetOriginalStep();
}

Double CData::GetStepTolerance()
{
	if (m_cData == NULL) {
		return 0.0;
	}
	return m_cData->GetStepTolerance();
}


//
//     Class CConnect
//

Boolean CConnect::StartListenToExit(CConnect^ cObj)
{
	return isl::CConnect::StartThreadListenToStop(cObj->GetConnect());
}

Boolean CConnect::SendStopRequest()
{
	return isl::CConnect::SendStopRequest();
}

Boolean CConnect::SendStopSession(String^ sSessionId)
{
	return isl::CConnect::SendStopSession(msclr::interop::marshal_as< std::string >(sSessionId));
}

CConnect::CConnect(Boolean bOwner)
{
	m_cConnect = new isl::CConnect(bOwner);
}

CConnect::~CConnect()
{
	delete m_cConnect;
}

isl::CConnect * CConnect::GetConnect()
{
	return m_cConnect;
}

void CConnect::CloseLogOnDelete(Boolean bVal)
{
	m_cConnect->CloseLogOnDelete(bVal);
}

void CConnect::SetMode(tStopMode eMode)
{
	m_cConnect->SetMode((isl::CConnect::tStopMode )eMode);
}

CConnect::tStopMode CConnect::GetMode()
{
	return (tStopMode )(m_cConnect->GetMode());
}

Boolean CConnect::ListenToExitSession()
{
	return m_cConnect->ListenToExitSession();
}

Boolean CConnect::SendStopSession()
{
	return m_cConnect->SendStopSession();
}

Boolean CConnect::IsTerminated()
{
	return m_cConnect->IsTerminated();
}

Boolean CConnect::New(String^ sName)
{
	return m_cConnect->New(msclr::interop::marshal_as< std::string >(sName));
}

Boolean CConnect::Load(String^ sFileName)
{
	bool bLoaded = m_cConnect->Load(msclr::interop::marshal_as< std::string >(sFileName));
	if (bLoaded) {
		for (int i = 0; i < m_cConnect->GetNbIOs(); i++) {
			isl::CData * cData = m_cConnect->GetIO(i);
			if (cData != 0) {
				CData^ cCLIData = gcnew CData(cData);
				m_lData.Add(cCLIData);
				if (cData->IsInput()) {
					m_lInputs.Add(cCLIData);
				}
				else if (cData->IsOutput()) {
					m_lOutputs.Add(cCLIData);
				}
			}
		}
	}
	return bLoaded;
}

Boolean CConnect::Check()
{
	return m_cConnect->Check();
}

Boolean CConnect::Save(String^ sFileName)
{
	return m_cConnect->Save(msclr::interop::marshal_as< std::string >(sFileName));
}

void CConnect::SetType(String^ sType)
{
	m_cConnect->SetType(msclr::interop::marshal_as< std::string >(sType));
}

void CConnect::SetStartTime(Double dVal)
{
	m_cConnect->SetStartTime(dVal);
}

Double CConnect::GetStartTime()
{
	return m_cConnect->GetStartTime();
}

void CConnect::SetEndTime(Double dVal)
{
	m_cConnect->SetEndTime(dVal);
}

Double CConnect::GetEndTime()
{
	return m_cConnect->GetEndTime();
}

void isl_cli::CConnect::SetStepSize(Double dVal)
{
	m_cConnect->SetStepSize(dVal);
}

Double CConnect::GetStepSize()
{
	return m_cConnect->GetStepSize();
}

CData^ CConnect::NewIO(String ^ sId, CData::tCausality eCausality, CData::tDataType eType, Int32 nSize)
{
	isl::CData * cData = m_cConnect->NewIO(msclr::interop::marshal_as< std::string >(sId),
		(isl::CVariable::tCausality)eCausality, (isl::CDataType::tType)eType, nSize);
	if (cData != NULL) {
		CData^ cCLIData = gcnew CData(cData);
		m_lData.Add(cCLIData);
		if (eCausality == CData::tCausality::CAUSALITY_INPUT) {
			m_lInputs.Add(cCLIData);
		}
		else if (eCausality == CData::tCausality::CAUSALITY_OUTPUT) {
			m_lOutputs.Add(cCLIData);
		}
		return cCLIData;
	}
	return nullptr;
}

Int32 CConnect::GetNbIOs()
{
	return m_cConnect->GetNbIOs();
}

CData^ CConnect::GetIO(String^ sId, Int32 % nInd)
{
	int nTmpInd = nInd;
	isl::CData * cData = m_cConnect->GetIO(
		msclr::interop::marshal_as< std::string >(sId), &nTmpInd);
	if (cData != NULL) {
		nInd = nTmpInd;
		return m_lData[nTmpInd];
	}
	return nullptr;
}

CData^ CConnect::GetIO(Int32 i)
{
	return m_lData[i];
}

Int32 CConnect::GetNbInputs()
{
	return m_cConnect->GetNbInputs();
}

CData^ CConnect::GetInput(Int32 i)
{
	return m_lInputs[i];
}

Int32 CConnect::GetNbOutputs()
{
	return m_cConnect->GetNbOutputs();
}

CData^ CConnect::GetOutput(Int32 i)
{
	return m_lOutputs[i];
}

Double CConnect::GetStepTolerance()
{
	return m_cConnect->GetStepTolerance();
}

Boolean isl_cli::CConnect::SetSessionId(String ^ sId)
{
	return m_cConnect->SetSessionId(msclr::interop::marshal_as< std::string >(sId));
}

String^ CConnect::GetSessionId()
{
	String^ sSessionId = gcnew String(m_cConnect->GetSessionId().c_str());
	return sSessionId;
}

Boolean CConnect::Create(String^ sSessionId)
{
	return m_cConnect->Create(msclr::interop::marshal_as< std::string >(sSessionId));
}

Boolean CConnect::Connect(Boolean bWait)
{
	return m_cConnect->Connect(bWait);
}

Boolean CConnect::Disconnect()
{
	return m_cConnect->Disconnect();
}

Boolean CConnect::OpenStore()
{
	// TODO: To implement
	return false;
}

Boolean CConnect::CloseStore()
{
	// TODO: To implement
	return false;
}

//
//     Class CTools
//

void CTools::PrintError(UInt32 uId, String^ sMsg)
{
	isl::CUtils::Error(uId, msclr::interop::marshal_as< std::string >(sMsg).c_str());
}

void CTools::PrintWarning(UInt32 uId, String^ sMsg)
{
	isl::CUtils::Warning(uId, msclr::interop::marshal_as< std::string >(sMsg).c_str());
}

void CTools::PrintInfo(UInt32 uId, String^ sMsg)
{
	isl::CUtils::Info(uId, msclr::interop::marshal_as< std::string >(sMsg).c_str());
}

UInt32 CTools::GetMaxStringSizeParameter()
{
	return ISLSims->GetMaxStringSize();
}

Int32 CTools::GetMaxNbSimulations()
{
	return ISLSims->GetMaxNb();
}

Boolean CTools::GetSimulation(Int32 nInd, UInt32 % uId, UInt32 % ulPID)
{
	Boolean bRet = ISLSims->Get(nInd);
	uId = ISLSims->GetId();
	ulPID = ISLSims->GetPID();
	return bRet;
}
