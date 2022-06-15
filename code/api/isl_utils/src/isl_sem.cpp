/*
 *     Name: isl_sem.cpp
 *
 *     Description: Semaphore tools.
 *
 *     Modification date: 20/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

/*
 *     Header files
 */

#ifdef WIN32
#include <windows.h>
#else
#include <semaphore.h>
#endif
#include "isl_sem.h"
#include "isl_misc.h"


/*
 *     Classes definition
 */

const std::string isl::CSem::c_sPrefix = "isl_syssem_";
const size_t isl::CSem::c_nPrefixMinSize = 3;

isl::CSem::CSem(const std::string & sKey, int nInitVal, tAccessMode eMode)
{
	m_sPrefix = c_sPrefix;
	SetKey(sKey, nInitVal, eMode);
}

isl::CSem::CSem(const std::string & sKey, const std::string & sPrefix, int nInitVal, tAccessMode eMode)
{
	m_sPrefix = c_sPrefix;
	SetPrefix(sPrefix);
	SetKey(sKey, nInitVal, eMode);
}

isl::CSem::~CSem()
{
	Close();
}

void isl::CSem::SetTimeout(int nTimeout)
{
	m_nTimeout = nTimeout;
}

void isl::CSem::SetPrefix(const std::string & sPrefix)
{
	if (sPrefix.size() >= c_nPrefixMinSize) {
		m_sPrefix = sPrefix;
	}
}

void isl::CSem::SetKey(const std::string & sKey, int nInitVal, tAccessMode eMode)
{
	if (sKey == m_sKey && eMode == OPEN) {
		return;
	}
	m_eStatus = SUCCESS;
	m_nError = 0;
	m_sKey = sKey;
	m_sName = BuildName();
	m_eMode = eMode;
	m_nInitVal = nInitVal;
	Close();
	Create();
}

std::string isl::CSem::GetKey()
{
	return m_sKey;
}

std::string isl::CSem::GetName()
{
	return m_sName;
}

bool isl::CSem::Acquire()
{
	return Modify(-1);
}

bool isl::CSem::Release(int n)
{
	if (n < 0) {
		return false;
	}
	if (n > 0) {
		return Modify(n);
	}
	return true;
}

isl::CSem::tStatus isl::CSem::GetStatus()
{
	return m_eStatus;
}

int isl::CSem::GetError()
{
	return m_nError;
}

std::string isl::CSem::BuildName()
{
	if (m_sKey.empty()) {
		m_eStatus = NOKEY;
		return std::string();
	}
#ifdef WIN32
	std::string sName;
#else
	std::string sName("/");
#endif
	sName += m_sPrefix; // Set the prefix
	std::string sTmpKey = isl::CString::ToWord(m_sKey);
	if (sTmpKey.empty()) {
		m_eStatus = WRONGKEYFORMAT;
		return std::string();
	}
	sName += sTmpKey;
	sName += isl::CString::Getsha1(m_sKey);
	return sName;
}

bool isl::CSem::Create()
{
	if (m_sKey.empty()) {
		m_eStatus = NOKEY;
		return false;
	}
#ifdef WIN32
	if (m_cSem == 0) {
		std::wstring swName(m_sName.begin(), m_sName.end());
		m_cSem = CreateSemaphore(0, m_nInitVal, MAXLONG, swName.c_str());
		if (m_cSem == 0) {
			m_eStatus = UNKNOWN;
			m_nError = GetLastError();
			switch (m_nError) {
				case ERROR_ACCESS_DENIED:
					m_eStatus = PERMISSIONDENIED;
					break;
				case ERROR_NO_SYSTEM_RESOURCES:
					m_eStatus = OUTOFRESOURCES;
					break;
				case ERROR_NOT_ENOUGH_MEMORY:
					m_eStatus = OUTOFMEMORY;
					break;
				default:
					break;
			}
			return false;
		}
	}
#else
	TODO;
#endif // WIN32
	return true;
}

bool isl::CSem::Close()
{
	// Do not update the status even on errors
#ifdef WIN32
	if (m_cSem != 0) {
		BOOL bVal = CloseHandle(m_cSem);
		m_cSem = 0;
		if (bVal == FALSE) {
			return false;
		}
	}
#else
	TODO;
#endif // WIN32
	return true;
}

bool isl::CSem::Modify(int n)
{
	if (m_cSem == 0) {
		return false;
	}
#ifdef WIN32
	if (n > 0) {
		// Release
		if (ReleaseSemaphore(m_cSem, n, 0) == 0) {
			m_eStatus = RELEASEFAILED;
			return false;
		}
	}
	else {
		// Acquire
		DWORD uTimeout = INFINITE;
		if (m_nTimeout > 0) {
			uTimeout = m_nTimeout;
		}
		DWORD ulRes = WaitForSingleObjectEx(m_cSem, uTimeout, FALSE);
		if (ulRes != WAIT_OBJECT_0) {
			m_eStatus = ACQUIREFAILED;
			if (ulRes == WAIT_TIMEOUT) {
				m_eStatus = TIMEOUTREACHED;
			}
			return false;
		}
	}
#else
	TODO;
#endif // WIN32
	return true;
}
