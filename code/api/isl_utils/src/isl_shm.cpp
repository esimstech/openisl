/*
 *     Name: isl_shm.cpp
 *
 *     Description: Shared Memory tools.
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

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <boost/filesystem.hpp>
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
		CSHMLocker(CSHM * cSHM) {
			m_cSHM = cSHM;
		}

		~CSHMLocker() {
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

isl::CSHM::CSHM(const std::string & sKey, bool bIsGlobal) : m_cSem(std::string())
{
	m_bIsGlobal = bIsGlobal;
	m_pData = 0;
	m_nSize = 0;
	m_eStatus = SUCCESS;
	m_nError = 0;
	m_cSHM = 0;
	m_bLockedByMe = false;
	m_bCreationMode = false;
	m_sPrefix = c_sPrefix;
	SetKey(sKey, bIsGlobal);
}

isl::CSHM::CSHM(const std::string & sKey, const std::string & sPrefix, bool bIsGlobal)
	: m_cSem(std::string())
{
	m_bIsGlobal = bIsGlobal;
	m_pData = 0;
	m_nSize = 0;
	m_eStatus = SUCCESS;
	m_nError = 0;
	m_cSHM = 0;
	m_bLockedByMe = false;
	m_bCreationMode = false;
	m_sPrefix = c_sPrefix;
	SetPrefix(sPrefix);
	SetKey(sKey, bIsGlobal);
}

isl::CSHM::~CSHM()
{
	if (IsAttached()) {
		Detach();
	}
	Close();
#ifndef WIN32
	if (m_cSHM != 0) {
		delete m_cSHM;
	}
	m_cSHM = 0;
#endif
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

void isl::CSHM::SetKey(const std::string & sKey, bool bIsGlobal)
{
	m_bIsGlobal = bIsGlobal;
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
	m_bCreationMode = false;
#ifdef WIN32
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
	if (m_cSHM == 0) {
		return false;
	}
	int nMode = 0600;
	if (eMode == READONLY) {
		nMode = 0400;
	}
	int nId = shmget(m_cSHM->nKey, 0, nMode);
	if (nId == -1) {
		m_eStatus = ATTACHERROR;
		m_nError = errno;
		return false;
	}
	nMode = 0;
	if (eMode == READONLY) {
		nMode = SHM_RDONLY;
	}
	m_pData = shmat(nId, 0, nMode);
	if (m_pData == (void *)-1) {
		m_eStatus = ATTACHERROR;
		m_nError = errno;
		return false;
	}
	shmid_ds stStats;
	if (shmctl(nId, IPC_STAT, &stStats) == 0) {
		m_nSize = (int)stStats.shm_segsz;
	}
	else {
		m_eStatus = ATTACHERROR;
		m_nError = errno;
		return false;
	}
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
	return Close();
#else
	if (shmdt(m_pData) == -1) {
		m_eStatus = DETACHERROR;
		m_nError = errno;
		switch (m_nError) {
		case EACCES:
			m_eStatus = PERMISSIONDENIED;
			break;
		case EINVAL:
		case ENOENT:
			m_eStatus = NOTFOUND;
			break;
		case EMFILE:
		case ENOSPC:
			m_eStatus = OUTOFRESOURCES;
			break;
		case ENOMEM:
			m_eStatus = OUTOFMEMORY;
			break;
		default:
			break;
		}
		return false;
	}
	m_pData = 0;
	m_nSize = 0;
	int nId = shmget(m_cSHM->nKey, 0, 0400);
	Close();
	shmid_ds stStats;
	if (shmctl(nId, IPC_STAT, &stStats) != 0) {
		switch (errno) {
		case EINVAL:
			return true;
		default:
			m_eStatus = DETACHERROR;
			m_nError = errno;
			return false;
		}
	}
	if (stStats.shm_nattch == 0) {
		if (shmctl(nId, IPC_RMID, &stStats) == -1) {
			switch (errno) {
			case EINVAL:
				return true;
			default:
				m_eStatus = REMOVEERROR;
				m_nError = errno;
				return false;
			}
		}
		if (boost::filesystem::remove(boost::filesystem::path(m_sName)) == false) {
			m_eStatus = FILEKEYNOTREMOVED;
			return false;
		}
	}
	return true;
#endif // WIN32
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
	if (m_bIsGlobal) {
		sName += isl::CIPCGlobal::Instance()->GetPrefix();
	}
#else
	std::string sName = boost::filesystem::temp_directory_path().string() + "/";
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
	if (m_cSHM == 0) {
		m_cSHM = new tsISLSHMHandle;
		m_cSHM->nKey = -1;
		m_cSHM->bFileCreated = false;
	}
	if (m_cSHM->nKey > 0) {
		return true;
	}
	if (boost::filesystem::exists(boost::filesystem::path(m_sName)) == false) {
		m_eStatus = NOTFOUND;
		return false;
	}
	m_cSHM->nKey = ftok(m_sName.c_str(), 73);
	if (m_cSHM->nKey == -1) {
		m_eStatus = KEYERROR;
		return false;
	}
	return true;
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
	LPSECURITY_ATTRIBUTES pSA = 0;
	SECURITY_ATTRIBUTES wSA = { 0 };
	if (m_bIsGlobal) {
		SECURITY_DESCRIPTOR wSD;
		if (isl::CIPCGlobal::Instance()->UpdateSecurity(&wSD, FILE_MAP_ALL_ACCESS)) {
			// Get security descriptor
			wSA.nLength = sizeof(wSA);
			wSA.lpSecurityDescriptor = &wSD;
			wSA.bInheritHandle = FALSE;
			pSA = &wSA;
		}
	}
	m_cSHM = CreateFileMapping(INVALID_HANDLE_VALUE, pSA, PAGE_READWRITE, 0, m_nSize, swName.c_str());
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
	if (m_cSHM == 0) {
		m_cSHM = new tsISLSHMHandle;
		m_cSHM->nKey = -1;
		m_cSHM->bFileCreated = false;
	}
	int nRet = isl::CFileSystem::CreateEmptyFile(m_sName);
	m_cSHM->bFileCreated = false;
	if (nRet == -1) {
		m_eStatus = KEYERROR;
		return false;
	}
	else if (nRet == 1) {
		m_cSHM->bFileCreated = true;
	}
	if (Get() == false) {
		if (m_cSHM->bFileCreated) {
			boost::filesystem::remove(boost::filesystem::path(m_sName));
			m_cSHM->bFileCreated = false;
		}
		return false;
	}
	if (shmget(m_cSHM->nKey, m_nSize, 0600 | IPC_CREAT | IPC_EXCL) == -1) {
		m_eStatus = UNKNOWN;
		m_nError = errno;
		switch (m_nError) {
		case EACCES:
			m_eStatus = PERMISSIONDENIED;
			break;
		case EINVAL:
			m_eStatus = INVALIDSIZE;
			break;
		case ENOENT:
			m_eStatus = NOTFOUND;
			break;
		case EMFILE:
		case ENOSPC:
			m_eStatus = OUTOFRESOURCES;
			break;
		case ENOMEM:
			m_eStatus = OUTOFMEMORY;
			break;
		case EEXIST:
			m_eStatus = ALREADYEXISTS;
			break;
		default:
			break;
		}
		if (m_cSHM->bFileCreated && m_eStatus != ALREADYEXISTS) {
			boost::filesystem::remove(boost::filesystem::path(m_sName));
			m_cSHM->bFileCreated = false;
		}
		return false;
	}
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
	if (m_cSHM != 0) {
		m_cSHM->nKey = 0;
		m_cSHM->bFileCreated = false;
	}
#endif // WIN32
	return true;
}