/*
 *     Name: isl_shm_connect.cpp
 *
 *     Description: ISL API SHM Connect class.
 *
 *     Modification date: 12/04/2019
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

/*
 *     Header files
 */

#include "isl_api.h"
#include "appsettings.h"
#include "isl_shm_connect.h"


/*
 *     Classes definition
 */

/*
 *     Class CSHMConnect
 */

int isl::CSHMConnect::GetSizeOf(CConnect * cConnect)
{
	unsigned int uMaxStrSize = CAppSettings().GetMaxSHMStringSize();
	int nSize = sizeof(unsigned int); // m_uType
	//
	nSize += sizeof(unsigned int); // m_uId
	nSize += sizeof(unsigned long); // m_ulPID
	//
	nSize += sizeof(int); // m_nSizeUUID
	nSize += sizeof(char) * uMaxStrSize; // m_sUUID
	//
	nSize += sizeof(int); // m_nSizeName
	nSize += sizeof(char) * uMaxStrSize; // m_sName
	//
	nSize += sizeof(int); // m_nSizeFile
	nSize += sizeof(char) * uMaxStrSize; // m_sFile
	//
	nSize += sizeof(int); // m_nNbData
	return nSize;
}

isl::CSHMConnect::CSHMConnect(void * pData, CConnect * cConnect)
{
	m_uType = NULL;
	m_uId = NULL;
	m_nSizeUUID = NULL;
	m_sUUID = NULL;
	m_nSizeName = NULL;
	m_sName = NULL;
	m_nSizeFile = NULL;
	m_sFile = NULL;
	m_nNbData = NULL;
	unsigned int uMaxStrSize = CAppSettings().GetMaxSHMStringSize();
	if (pData != NULL) {
		m_uType = (unsigned int *)pData;
		char * pNext = (char *)pData + sizeof(unsigned int);
		//
		m_uId = (unsigned int *)pNext;
		pNext = pNext + sizeof(unsigned int);
		//
		m_ulPID = (unsigned long *)pNext;
		pNext = pNext + sizeof(unsigned long);
		//
		m_nSizeUUID = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_sUUID = (char *)pNext;
		pNext = pNext + (sizeof(char) * uMaxStrSize);
		//
		m_nSizeName = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_sName = (char *)pNext;
		pNext = pNext + (sizeof(char) * uMaxStrSize);
		//
		m_nSizeFile = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_sFile = (char *)pNext;
		pNext = pNext + (sizeof(char) * uMaxStrSize);
		//
		m_nNbData = (int *)pNext;
	}
	m_cParent = cConnect;
}

isl::CSHMConnect::~CSHMConnect()
{
	m_uType = NULL;
	m_uId = NULL;
	m_nSizeUUID = NULL;
	m_sUUID = NULL;
	m_nSizeName = NULL;
	m_sName = NULL;
	m_nSizeFile = NULL;
	m_sFile = NULL;
	m_nNbData = NULL;
	m_cParent = NULL;
}

bool isl::CSHMConnect::Initialize()
{
	if (m_cParent == 0) {
		return false;
	}
	if (m_uType == 0) { // Test the first element
		return false;
	}
	unsigned int uMaxStrSize = CAppSettings().GetMaxSHMStringSize();
	*m_uType = m_cParent->GetType();
	*m_uId = m_cParent->GetUId();
	*m_ulPID = m_cParent->GetPID();
	//
	int nSize = m_cParent->GetId().size();
	if ((unsigned int)nSize > uMaxStrSize) {
		nSize = uMaxStrSize;
	}
	*m_nSizeUUID = nSize;
	if (nSize >= 0) {
		memcpy(m_sUUID, m_cParent->GetId().c_str(), nSize);
		m_sUUID[nSize] = '\0';
	}
	//
	nSize = m_cParent->GetName().size();
	if ((unsigned int)nSize > uMaxStrSize) {
		nSize = uMaxStrSize;
	}
	*m_nSizeName = nSize;
	if (nSize >= 0) {
		memcpy(m_sName, m_cParent->GetName().c_str(), nSize);
		m_sName[nSize] = '\0';
	}
	//
	nSize = m_cParent->GetFileName().size();
	if ((unsigned int)nSize > uMaxStrSize) {
		nSize = uMaxStrSize;
	}
	*m_nSizeFile = nSize;
	if (nSize >= 0) {
		memcpy(m_sFile, m_cParent->GetFileName().c_str(), nSize);
		m_sFile[nSize] = '\0';
	}
	//
	*m_nNbData = m_cParent->GetNbIOs();
	return true;
}

unsigned int isl::CSHMConnect::GetType()
{
	return *m_uType;
}

unsigned int isl::CSHMConnect::GetId()
{
	return *m_uId;
}

unsigned long isl::CSHMConnect::GetPID()
{
	return *m_ulPID;
}

std::string isl::CSHMConnect::GetUUID()
{
	return std::string(m_sUUID);
}

std::string isl::CSHMConnect::GetName()
{
	return std::string(m_sName);
}

std::string isl::CSHMConnect::GetFile()
{
	return std::string(m_sFile);
}

int isl::CSHMConnect::GetNbData()
{
	return *m_nNbData;
}
