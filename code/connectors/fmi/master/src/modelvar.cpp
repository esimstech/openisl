/*
 *     Name: modelvar.cpp
 *
 *     Description: Model Variable class.
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

#include <isl_log.h>

#include "model.h"
#include "modelvar.h"

/*
 *     Classes definition
 */

/*
 *     Class CModelVar
 */

CModelVar::CModelVar(isl::CData * cIO, isl::CXMLNode * cVar, CModel * cParent)
{
	m_cVar = cVar;
	m_cIO = cIO;
	m_cSlave = 0;
	if (cParent != 0) {
		m_cSlave = cParent->GetSlave();
	}
	m_eType = isl::CDataType::TP_UNKNOWN;
	m_bStore = false;
	if (m_cIO != 0) {
		if (m_cIO->GetType() != 0) {
			m_eType = m_cIO->GetType()->GetId();
		}
	}
	std::string sRef = m_cVar->GetAttribute("valueReference");
	m_uRef = std::stoul(sRef);
	// Warning m_cIO (XML model file) is different than m_cVar (FMU)
	m_bStore = m_cIO->IsStoreUsed();
	if (m_bStore) {
		AppLogInfo(INFO_MDLVAR_ISSTORED, "Values of '%s' will be stored.",
			m_cIO->GetId().c_str());
	}
	m_nSize = m_cIO->GetType()->GetSize();
	if (m_nSize <= 0) {
		m_nSize = 1;
	}
	m_pData = NULL;
	switch (m_eType) {
		case isl::CDataType::TP_BOOLEAN:
			m_pData = calloc(m_nSize, sizeof(bool));
			break;
		case isl::CDataType::TP_INTEGER:
			m_pData = calloc(m_nSize, sizeof(int));
			break;
		case isl::CDataType::TP_REAL:
			m_pData = calloc(m_nSize, sizeof(double));
			break;
		case isl::CDataType::TP_STRING:
			m_pData = calloc(m_nSize, sizeof(char));
			break;
		default:
			break;
	}
}

CModelVar::~CModelVar()
{
	m_cVar = 0;
	m_cIO = 0;
	m_cSlave = 0;
	if (m_pData != 0) {
		free(m_pData);
	}
	m_pData = 0;
}

isl::CData * CModelVar::GetIO()
{
	return m_cIO;
}

bool CModelVar::Validate(int & nErrorCode)
{
	if (m_cSlave == 0) {
		nErrorCode = 1;
		return false;
	}
	if (m_cIO == 0) {
		nErrorCode = 2;
		return false;
	}
	if (m_cVar == 0) {
		nErrorCode = 3;
		return false;
	}
	if (m_cIO->GetType() == 0) {
		nErrorCode = 4;
		return false;
	}
	std::string sIOType = m_cIO->GetType()->GetIdAsStr();
	if (m_cVar->GetNode(sIOType) == 0) {
		nErrorCode = 5;
		return false;
	}
	if (m_cIO->GetType()->GetSize() != 1) {
		// Shall be 1
		nErrorCode = 6;
		return false;
	}
	nErrorCode = 0;
	return true;
}

bool CModelVar::InitializeModel()
{
	AppLogInfo(INFO_MDLVAR_INITIALIZEMDL, "Initializing model variable %s...",
		m_cVar->GetAttribute("name").c_str());
	switch (m_eType) {
		case isl::CDataType::TP_BOOLEAN:
		{
			std::string sStartVal(m_cVar->GetNode("Boolean")->GetAttribute("start"));
			bool bVal = sStartVal == "true";
			return m_cSlave->SetBoolean(m_uRef, &bVal);
		}
		case isl::CDataType::TP_INTEGER:
		{
			std::string sStartVal(m_cVar->GetNode("Integer")->GetAttribute("start"));
			int nVal = std::stoi(sStartVal); // could be replaced by boost::lexical_cast
			return m_cSlave->SetInteger(m_uRef, &nVal);
		}
		case isl::CDataType::TP_REAL:
		{
			std::string sStartVal(m_cVar->GetNode("Real")->GetAttribute("start"));
			double dVal = std::stod(sStartVal); // could be replaced by boost::lexical_cast
			return m_cSlave->SetReal(m_uRef, &dVal);
		}
		case isl::CDataType::TP_STRING:
		{
			std::string sStartVal(m_cVar->GetNode("String")->GetAttribute("start"));
			return m_cSlave->SetString(m_uRef, (char * )(sStartVal.c_str()));
		}
		default:
			return false;
	}
	return true;
}

bool CModelVar::InitializeISL(double dTime)
{
	if (m_cIO->IsInput() == false) {
		return true;
	}
	AppLogInfo(INFO_MDLVAR_INITIALIZEISL, "Initializing ISL variable %s...",
		m_cIO->GetId().c_str());
	return m_cIO->Initialize(dTime);
}

bool CModelVar::TransferDataISLToModel(double dTime)
{
	// Get data from ISL
	double dNewTime = dTime;
	if (m_cIO->GetData(m_pData, &dNewTime, dTime, true) == false) {
		return false;
	}
	if (m_bStore) {
		m_cIO->StoreData(m_pData, dNewTime);
	}
	// Set data to the FMU
	bool bRet = true;
	switch (m_eType) {
		case isl::CDataType::TP_BOOLEAN:
			bRet = m_cSlave->SetBoolean(m_uRef, (bool *)m_pData);
			break;
		case isl::CDataType::TP_INTEGER:
			bRet = m_cSlave->SetInteger(m_uRef, (int *)m_pData);
			break;
		case isl::CDataType::TP_REAL:
			bRet = m_cSlave->SetReal(m_uRef, (double *)m_pData);
			break;
		case isl::CDataType::TP_STRING:
			bRet = m_cSlave->SetString(m_uRef, (char *)m_pData);
			break;
		default:
			return false;
	}
	return bRet;
}

bool CModelVar::TransferDataModelToISL(double dTime)
{
	// Get data from the FMU
	bool bRet = true;
	switch (m_eType) {
		case isl::CDataType::TP_BOOLEAN:
			bRet = m_cSlave->GetBoolean(m_uRef, (bool *)m_pData);
			break;
		case isl::CDataType::TP_INTEGER:
			bRet = m_cSlave->GetInteger(m_uRef, (int *)m_pData);
			break;
		case isl::CDataType::TP_REAL:
			bRet = m_cSlave->GetReal(m_uRef, (double *)m_pData);
			break;
		case isl::CDataType::TP_STRING:
			bRet = m_cSlave->GetString(m_uRef, (char *)m_pData);
			break;
		default:
			return false;
	}
	if (bRet == false) {
		return false;
	}
	// Set data to ISL
	if (m_cIO->SetData(m_pData, dTime, true) == false) {
		return false;
	}
	if (m_bStore) {
		m_cIO->StoreData(m_pData, dTime);
	}
	return true;
}
