/*
 *     Name: isl_shm.cpp
 *
 *     Description: Shared Memory tools.
 *
 *     Modification date: 24/02/2022
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
#endif
#include "isl_shm.h"
#include "isl_misc.h"


/*
 *     Classes definition
 */

namespace isl {
	class CSHMLocker
	{
	public:
		CSHMLocker::CSHMLocker(CSHM * cSHM) {
			m_cSHM = cSHM;
		}

		CSHMLocker::~CSHMLocker() {
			if (m_cSHM != 0) {
				m_cSHM->Unlock();
			}
		}

		bool Lock() {
			if (m_cSHM != 0) {
				if (m_cSHM->Lock()) {
					return true;
				}
			}
			m_cSHM = 0;
			return false;
		}

	private:
		CSHM * m_cSHM;
	};
}

const std::string isl::CSHM::c_sPrefix = "isl_sysshm_";
const size_t isl::CSHM::c_nPrefixMinSize = 3;

isl::CSHM::CSHM(const std::string & sKey) : m_cSem(std::string())
{
	m_pData = 0;
	m_nSize = 0;
	m_eStatus = SUCCESS;
	m_nError = 0;
	m_cSHM = 0;
	m_bLockedByMe = false;
	m_bCreationMode = false;
	m_sPrefix = c_sPrefix;
	SetKey(sKey);
}

isl::CSHM::CSHM(const std::string & sKey, const std::string & sPrefix)
	: m_cSem(std::string())
{
	m_pData = 0;
	m_nSize = 0;
	m_eStatus = SUCCESS;
	m_nError = 0;
	m_cSHM = 0;
	m_bLockedByMe = false;
	m_bCreationMode = false;
	m_sPrefix = c_sPrefix;
	SetPrefix(sPrefix);
	SetKey(sKey);
}

isl::CSHM::~CSHM()
{
	if (IsAttached()) {
		Detach();
	}
	Close();
}

isl::CSHM::tStatus isl::CSHM::GetStatus()
{
	return m_eStatus;
}

int isl::CSHM::GetError()
{
	return m_nError;
}

isl::CSem::tStatus isl::CSHM::GetSemStatus()
{
	return m_cSem.GetStatus();
}

void isl::CSHM::SetPrefix(const std::string & sPrefix)
{
	if (sPrefix.size() >= c_nPrefixMinSize) {
		m_sPrefix = sPrefix;
	}
}

void isl::CSHM::SetKey(const std::string & sKey)
{
	if (sKey == m_sKey) {
		return;
	}
	if (sKey.empty()) {
		m_eStatus = NOKEY;
		return;
	}
	m_eStatus = SUCCESS;
	if (IsAttached()) {
		Detach();
	}
	Close();
	m_sKey = sKey;
	m_sName = BuildName();
}

std::string isl::CSHM::GetKey()
{
	return m_sKey;
}

std::string isl::CSHM::GetName()
{
	return m_sName;
}

bool isl::CSHM::Create(int nSize, tAccessMode eMode)
{
	if (Init() == false) {
		return false;
	}
	if (m_sKey.empty()) {
		m_eStatus = NOKEY;
		return false;
	}
	CSHMLocker cLocker(this);
	if (cLocker.Lock() == false) {
		m_eStatus = LOCKCREATEERROR;
		return false;
	}
	if (nSize <= 0) {
		m_eStatus = INVALIDSIZE;
		return false;
	}
	m_nSize = nSize;
	if (Create() == false) {
		return false;
	}
	m_bCreationMode = true;
	if (Attach(eMode) == false) {
		return false;
	}
	return true;
}

int isl::CSHM::Size()
{
	return m_nSize;
}

bool isl::CSHM::Attach(tAccessMode eMode)
{
	if (m_bCreationMode == false) {
		if (IsAttached()) {
			return false;
		}
		if (Init() == false) {
			return false;
		}
		if (m_sKey.empty()) {
			m_eStatus = NOKEY;
			return false;
		}
		CSHMLocker cLocker(this);
		if (cLocker.Lock() == false) {
			m_eStatus = LOCKATTACHERROR;
			return false;
		}
		if (Get() == false) {
			return false;
		}
	}
#ifdef WIN32
	m_bCreationMode = false;
	int nPerms = FILE_MAP_ALL_ACCESS;
	if (eMode == READONLY) {
		nPerms = FILE_MAP_READ;
	}
	m_pData = (void *)MapViewOfFile(m_cSHM, nPerms, 0, 0, 0);
	if (m_pData == 0) {
		Close();
		m_eStatus = ATTACHERROR;
		m_nError = GetLastError();
		return false;
	}
	MEMORY_BASIC_INFORMATION stInfo;
	if (VirtualQuery(m_pData, &stInfo, sizeof(stInfo)) == 0) {
		m_eStatus = GETSIZEERROR;
		return false;
	}
	m_nSize = (int )(stInfo.RegionSize);
#else
	TODO;
#endif // WIN32
	return true;
}

bool isl::CSHM::IsAttached()
{
	return (m_pData != 0);
}

bool isl::CSHM::Detach()
{
	if (IsAttached() == false) {
		return false;
	}
	if (m_sKey.empty()) {
		m_eStatus = NOKEY;
		return false;
	}
	CSHMLocker cLocker(this);
	if (cLocker.Lock() == false) {
		m_eStatus = LOCKDETACHERROR;
		return false;
	}
#ifdef WIN32
	if (UnmapViewOfFile(m_pData) == 0) {
		m_eStatus = DETACHERROR;
		m_nError = GetLastError();
		return false;
	}
	m_pData = 0;
	m_nSize = 0;
#else
#endif // WIN32
	return Close();
}

void * isl::CSHM::Data()
{
	return m_pData;
}

bool isl::CSHM::Lock()
{
	if (m_bLockedByMe) {
		return true;
	}
	if (m_cSem.Acquire() == false) {
		m_eStatus = LOCKERROR;
		return false;
	}
	m_bLockedByMe = true;
	return true;
}

bool isl::CSHM::Unlock()
{
	if (m_bLockedByMe == false) {
		return true;
	}
	m_bLockedByMe = false;
	if (m_cSem.Release() == false) {
		m_eStatus = UNLOCKERROR;
		return false;
	}
	return true;
}

std::string isl::CSHM::BuildName()
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

bool isl::CSHM::Init()
{
	if (Close() == false) {
		return false;
	}
	m_cSem.SetKey(std::string(), 1);
	m_cSem.SetKey(m_sKey, 1);
	if (m_cSem.GetStatus() != isl::CSem::SUCCESS) {
		m_eStatus = SEMAPHOREERROR;
		return false;
	}
	return true;
}

bool isl::CSHM::Get()
{
#ifdef WIN32
	if (m_cSHM == 0) {
		if (m_sName.empty()) {
			m_eStatus = NOINTKEY;
			return false;
		}
		std::wstring swName(m_sName.begin(), m_sName.end());
		m_cSHM = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, swName.c_str());
		if (m_cSHM == 0) {
			m_eStatus = UNKNOWN;
			m_nError = GetLastError();
			switch (m_nError) {
				case ERROR_ALREADY_EXISTS:
					m_eStatus = ALREADYEXISTS;
					break;
				case ERROR_FILE_NOT_FOUND:
					m_eStatus = NOTFOUND;
					break;
				case ERROR_COMMITMENT_LIMIT:
					m_eStatus = INVALIDSIZE;
					break;
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

bool isl::CSHM::Create()
{
	if (m_sName.empty()) {
		m_eStatus = NOINTKEY;
		return false;
	}
#ifdef WIN32
	std::wstring swName(m_sName.begin(), m_sName.end());
	m_cSHM = CreateFileMapping(INVALID_HANDLE_VALUE, 0, PAGE_READWRITE, 0, m_nSize, swName.c_str());
	if (m_cSHM == 0) {
		m_eStatus = UNKNOWN;
		m_nError = GetLastError();
		switch (m_nError) {
		case ERROR_ALREADY_EXISTS:
			m_eStatus = ALREADYEXISTS;
			break;
		case ERROR_FILE_NOT_FOUND:
			m_eStatus = NOTFOUND;
			break;
		case ERROR_COMMITMENT_LIMIT:
			m_eStatus = INVALIDSIZE;
			break;
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
#else
	TODO;
#endif // WIN32
	return true;
}

bool isl::CSHM::Close()
{
	// Do not update the status even on errors
#ifdef WIN32
	if (m_cSHM != 0) {
		BOOL bVal = CloseHandle(m_cSHM);
		m_cSHM = 0;
		if (bVal == FALSE) {
			return false;
		}
	}
#else
	TODO;
#endif // WIN32
	return true;
}