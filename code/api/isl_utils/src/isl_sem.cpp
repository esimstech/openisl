/*
 *     Name: isl_sem.cpp
 *
 *     Description: Semaphore tools.
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
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <fcntl.h>
#include <errno.h>
#include <boost/filesystem.hpp>
#endif
#include <vector>
#include "isl_sem.h"
#include "isl_misc.h"


/*
 *     Macros definition
 */

#if 0
#define IMPLEMENT_ADJUSTTOKEN
#endif


/*
 *     Types definition
 */

#ifndef WIN32
union semun {
	int val;
	struct semid_ds * buf;
	unsigned short * array;
	struct seminfo * __buf;
};
#endif


/*
 *     Classes declarations
 */

namespace isl {
	class CIPCGlobalPrivate
	{
	public:
		CIPCGlobalPrivate(CIPCGlobal::tStatus * eStatus, int * nError);
		~CIPCGlobalPrivate();

		bool UpdateSecurity(void * pSD, unsigned long ulAccessMask);

	private:
#ifdef WIN32
		HANDLE m_hToken;
		TOKEN_PRIVILEGES m_tpCurrent;
		TOKEN_PRIVILEGES m_tpPrevious;
		DWORD m_dwSize;
		LUID m_wLuid;

		std::vector<PACL> m_lHeaps;
#endif
		CIPCGlobal::tStatus * m_eStatus;
		int * m_nError;
	};
}

/*
 *     Classes definition
 */

/*
 *     Class CIPCGlobalPrivate
 */

isl::CIPCGlobalPrivate::CIPCGlobalPrivate(CIPCGlobal::tStatus * eStatus, int * nError)
{
	m_eStatus = eStatus;
	m_nError = nError;
#if defined(WIN32) && defined(IMPLEMENT_ADJUSTTOKEN)
	m_dwSize = sizeof(TOKEN_PRIVILEGES);
	//
	// Step 1
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &m_hToken)) {
		*m_eStatus = CIPCGlobal::OPEN_FAILED;
		*m_nError = GetLastError();
		return;
	}
	//
	// Step 2
	if (!LookupPrivilegeValue(NULL, SE_CREATE_GLOBAL_NAME, &m_wLuid))	{
		*m_eStatus = CIPCGlobal::LOOKUP_FAILED;
		*m_nError = GetLastError();
		CloseHandle(m_hToken);
		return;
	}
	ZeroMemory(&m_tpCurrent, sizeof(m_tpCurrent));
	m_tpCurrent.PrivilegeCount = 1;
	m_tpCurrent.Privileges[0].Luid = m_wLuid;
	m_tpCurrent.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	//
	// Step 3
	// Adjust Token privileges
	if (!AdjustTokenPrivileges(m_hToken, FALSE, &m_tpCurrent,
		sizeof(TOKEN_PRIVILEGES), &m_tpPrevious, &m_dwSize)) {
		*m_eStatus = CIPCGlobal::ADJUST_FAILED;
		*m_nError = GetLastError();
		CloseHandle(m_hToken);
		return;
	}
#endif
}

isl::CIPCGlobalPrivate::~CIPCGlobalPrivate()
{
#if defined(WIN32) && defined(IMPLEMENT_ADJUSTTOKEN)
	// End Step
	// Disable privilege again
	AdjustTokenPrivileges(m_hToken, FALSE, &m_tpPrevious, m_dwSize, NULL, NULL);
	if (GetLastError() != ERROR_SUCCESS) {
		*m_eStatus = CIPCGlobal::RESTORE_FAILED;
		*m_nError = GetLastError();
		CloseHandle(m_hToken);
		return;
	}
	CloseHandle(m_hToken);
#endif
#ifdef WIN32
	if (m_lHeaps.empty() == false) {
		for (std::size_t i = 0; i < m_lHeaps.size(); i++) {
			if (m_lHeaps[i] != 0) {
				HeapFree(GetProcessHeap(), 0, m_lHeaps[i]);
			}
		}
		m_lHeaps.clear();
	}
#endif
	m_eStatus = 0;
	m_nError = 0;
}

bool isl::CIPCGlobalPrivate::UpdateSecurity(void * pSD, unsigned long ulAccessMask)
{
#ifdef WIN32
	PSECURITY_DESCRIPTOR pwSD = pSD;
	DWORD  dwAclLength = 0;
	PSID   pAuthenticatedUsersSID = 0;

	PACL   pDACL = 0;
	BOOL   bResult = FALSE;

	PACCESS_ALLOWED_ACE pACE = 0;
	SID_IDENTIFIER_AUTHORITY SecIdAuth = SECURITY_NT_AUTHORITY;
	SECURITY_INFORMATION SecInformation = DACL_SECURITY_INFORMATION;

	try {
		// Initialize the security descriptor  
		if (!InitializeSecurityDescriptor(pwSD, SECURITY_DESCRIPTOR_REVISION)) {
			throw - 1;
		}

		// Obtain a SID for the Authenticated Users Group  
		if (!AllocateAndInitializeSid(&SecIdAuth, 1, SECURITY_AUTHENTICATED_USER_RID,
				0, 0, 0, 0, 0, 0, 0, &pAuthenticatedUsersSID)) {
			throw -1;
		}

		// The Authenticated Users group includes all user accounts that  
		// have been successfully authenticated by the system. If access  
		// must be restricted to a specific user or group other than  
		// Authenticated Users, the SID can be constructed using the  
		// LookupAccountSid() API based on a user or group name.  

		// Calculate the DACL length  
		dwAclLength = sizeof(ACL) +
			// Add space for Authenticated Users group ACE  
			sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) +
			GetLengthSid(pAuthenticatedUsersSID);

		// Allocate memory for the DACL
		pDACL = (PACL)HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, dwAclLength);
		if (pDACL == 0) {
			throw - 1;
		}

		// Initialize the DACL  
		if (!InitializeAcl(pDACL, dwAclLength, ACL_REVISION)) {
			throw - 1;
		}

		// Add the Authenticated Users group ACE to the DACL with  
		// ulAccessMask = SEMAPHORE_ALL_ACCESS access for semaphores or
		// FILE_MAP_ALL_ACCESS == SECTION_ALL_ACCESS access for shared memories
		if (!AddAccessAllowedAce(pDACL, ACL_REVISION, ulAccessMask, pAuthenticatedUsersSID)) {
			throw - 1;
		}

		// Set the DACL in the security descriptor  
		if (!SetSecurityDescriptorDacl(pwSD, TRUE, pDACL, FALSE)) {
			throw - 1;
		}

		bResult = TRUE;
	}
	catch (int) {
		if (pAuthenticatedUsersSID) {
			FreeSid(pAuthenticatedUsersSID);
		}
	}

	if (bResult == FALSE) {
		if (pDACL != 0) {
			HeapFree(GetProcessHeap(), 0, pDACL);
			pDACL = 0;
		}
	}
	else if (pDACL != 0) {
		m_lHeaps.push_back(pDACL);
	}
#endif
	return true;
}

/*
 *     Class CIPCGlobal
 */

const std::string isl::CIPCGlobal::c_sPrefix = "Global\\";
isl::CIPCGlobal::CMngtIPCGlobal isl::CIPCGlobal::m_cMngtInstance;

isl::CIPCGlobal::CMngtIPCGlobal::CMngtIPCGlobal()
{
	m_cInstance = new CIPCGlobal;
}

isl::CIPCGlobal::CMngtIPCGlobal::~CMngtIPCGlobal()
{
	delete m_cInstance;
}

isl::CIPCGlobal * isl::CIPCGlobal::CMngtIPCGlobal::Get()
{
	return m_cInstance;
}

isl::CIPCGlobal::CIPCGlobal()
{
	m_eStatus = SUCCESS;
	m_nError = 0;
	m_cPrivate = new CIPCGlobalPrivate(&m_eStatus, &m_nError);
}

isl::CIPCGlobal::~CIPCGlobal()
{
	if (m_cPrivate != 0) {
		delete m_cPrivate;
	}
	m_cPrivate = 0;
}

bool isl::CIPCGlobal::UpdateSecurity(void * pSD, unsigned long ulAccessMask)
{
	return m_cPrivate->UpdateSecurity(pSD, ulAccessMask);
}

const std::string isl::CIPCGlobal::GetPrefix()
{
	return c_sPrefix;
}

isl::CIPCGlobal::tStatus isl::CIPCGlobal::GetStatus()
{
	return m_eStatus;
}

int isl::CIPCGlobal::GetError()
{
	return m_nError;
}

isl::CIPCGlobal * isl::CIPCGlobal::Instance()
{
	return m_cMngtInstance.Get();
}

/*
 *     Class CSem
 */

const std::string isl::CSem::c_sPrefix = "isl_syssem_";
const size_t isl::CSem::c_nPrefixMinSize = 3;

isl::CSem::CSem(const std::string & sKey, int nInitVal, tAccessMode eMode, bool bIsGlobal)
{
	m_bIsGlobal = bIsGlobal;
	m_cSem = 0;
	m_sPrefix = c_sPrefix;
	SetKey(sKey, nInitVal, eMode, bIsGlobal);
}

isl::CSem::CSem(const std::string & sKey, const std::string & sPrefix, int nInitVal, tAccessMode eMode, bool bIsGlobal)
{
	m_bIsGlobal = bIsGlobal;
	m_cSem = 0;
	m_sPrefix = c_sPrefix;
	SetPrefix(sPrefix);
	SetKey(sKey, nInitVal, eMode, bIsGlobal);
}

isl::CSem::~CSem()
{
	Close();
#ifndef WIN32
	if (m_cSem != 0) {
		delete m_cSem;
	}
	m_cSem = 0;
#endif
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

void isl::CSem::SetKey(const std::string & sKey, int nInitVal, tAccessMode eMode, bool bIsGlobal)
{
	if (sKey == m_sKey && eMode == OPEN) {
		return;
	}
#ifndef WIN32
	if (m_cSem != 0) {
		if ((m_sKey == sKey) && (eMode == CREATE)) {
			if (m_cSem->bSemCreated && m_cSem->bFileCreated) {
				m_nInitVal = nInitVal;
				m_cSem->nKey = -1;
				Create(eMode);
			}
		}
	}
#endif
	m_bIsGlobal = bIsGlobal;
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

bool isl::CSem::Create(tAccessMode eMode)
{
	if (m_sKey.empty()) {
		m_eStatus = NOKEY;
		return false;
	}
#ifdef WIN32
	if (m_cSem == 0) {
		std::wstring swName(m_sName.begin(), m_sName.end());
		LPSECURITY_ATTRIBUTES pSA = 0;
		SECURITY_ATTRIBUTES wSA = { 0 };
		if (m_bIsGlobal) {
			SECURITY_DESCRIPTOR wSD;
			if (isl::CIPCGlobal::Instance()->UpdateSecurity(&wSD, SEMAPHORE_ALL_ACCESS)) {
				// Get security descriptor
				wSA.nLength = sizeof(wSA);
				wSA.lpSecurityDescriptor = &wSD;
				wSA.bInheritHandle = FALSE;
				pSA = &wSA;
			}
		}
		m_cSem = CreateSemaphore(pSA, m_nInitVal, MAXLONG, swName.c_str());
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
	if (m_cSem == 0) {
		m_cSem = new tsISLSemHandle;
		m_cSem->nKey = -1;
		m_cSem->nSemaphore = -1;
		m_cSem->bFileCreated = false;
		m_cSem->bSemCreated = false;
	}
	if (m_cSem != 0) {
		if (m_cSem->nKey != -1) {
			return true;
		}
		int nRet = isl::CFileSystem::CreateEmptyFile(m_sName);
		m_cSem->bFileCreated = false;
		if (nRet == -1) {
			m_eStatus = KEYERROR;
			return false;
		}
		else if (nRet == 1) {
			m_cSem->bFileCreated = true;
		}
		m_cSem->nKey = ftok(m_sName.c_str(), 73);
		if (m_cSem->nKey == -1) {
			m_eStatus = KEYERROR;
			return false;
		}
		m_cSem->nSemaphore = semget(m_cSem->nKey, 1, 0600 | IPC_CREAT | IPC_EXCL);
		if (m_cSem->nSemaphore == -1) {
			if (errno == EEXIST) {
				m_cSem->nSemaphore = semget(m_cSem->nKey, 1, 0600 | IPC_CREAT);
			}
			if (m_cSem->nSemaphore == -1) {
				m_eStatus = HANDLEFAILED;
				m_nError = errno;
				Close();
				return false;
			}
		}
		else {
			m_cSem->bSemCreated = true;
			m_cSem->bFileCreated = true;
		}
		if (eMode == CREATE) {
			m_cSem->bSemCreated = true;
			m_cSem->bFileCreated = true;
		}
		if (m_cSem->bSemCreated && m_nInitVal >= 0) {
			semun uInitValOp;
			uInitValOp.val = m_nInitVal;
			if (semctl(m_cSem->nSemaphore, 0, SETVAL, uInitValOp) == -1) {
				m_eStatus = HANDLEFAILED;
				m_nError = errno;
				Close();
				return false;
			}
		}
		return true;
	}
	return false;
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
	if (m_cSem != 0) {
		bool bRet = true;
		m_cSem->nKey = -1;
		if (m_cSem->bFileCreated) {
			boost::filesystem::remove(boost::filesystem::path(m_sName));
			m_cSem->bFileCreated = false;
		}
		if (m_cSem->bSemCreated) {
			if (m_cSem->nSemaphore != -1) {
				if (semctl(m_cSem->nSemaphore, 0, IPC_RMID, 0) == -1) {
					m_eStatus = CLOSEFAILED;
					bRet = false;
				}
				m_cSem->nSemaphore = -1;
			}
			m_cSem->bSemCreated = false;
		}
		return bRet;
	}
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
	struct sembuf stOp;
	stOp.sem_num = 0;
	stOp.sem_op = n;
	stOp.sem_flg = SEM_UNDO;
	//
	struct timespec * stWait = 0;
	if (m_nTimeout > 0) {
		stWait = new timespec;
		stWait->tv_sec = m_nTimeout * 1000;
		stWait->tv_nsec = 0;
	}
	int nRet = -1;
	do {
		nRet = semtimedop(m_cSem->nSemaphore, &stOp, 1, stWait);
	} while (nRet == -1 && errno == EINTR);
	if (stWait != 0) {
		delete stWait;
		stWait = 0;
	}
	if (nRet == -1) {
		if (errno == EINVAL || errno == EIDRM) {
			m_cSem->nSemaphore = -1;
			Close();
			Create();
			return Modify(n);
		}
		m_eStatus = MODIFYFAILED;
		m_nError = errno;
		return false;
	}
#endif // WIN32
	m_eStatus = SUCCESS;
	m_nError = 0;
	return true;
}
