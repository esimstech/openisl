/*
 *     Name: isl_simulations.cpp
 *
 *     Description: ISL simulation instances management.
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

#include <boost/format.hpp>

#include <isl_log.h>
#include "isl_api.h"
#include <isl_shm.h>
#include "isl_shm_connect.h"
#include "isl_settings.h"
#include "isl_errorcodes.h"


/*
 *     Constants definition
 */

#define SHM_ISL_KEY_ID			"_isl_shm_gen%1%"

const std::string isl::CSimulations::c_sKeyGeneralId = "7HZ5IPtt27157jTz";


/*
 *     Classes definition
 */

isl::CSimulations * isl::CSimulations::m_cInstance = NULL;

isl::CSimulations::CSimulations()
{
	//
	m_uMaxStrSize = CAppSettings().GetMaxSHMStringSize();
	m_nMaxNbSims = CAppSettings().GetMaxNbRunSims();
	//
	m_nSizeId = sizeof(unsigned int);
	m_nSize = m_nSizeId;
	m_nSizePID = sizeof(unsigned long);
	m_nSize += m_nSizePID;
	m_nSizeSizeName = sizeof(int);
	m_nSize += m_nSizeSizeName;
	m_nSizeName = sizeof(char) * m_uMaxStrSize;
	m_nSize += m_nSizeName;
	m_nSizeSizeSession = sizeof(int);
	m_nSize += m_nSizeSizeSession;
	m_nSizeSession = sizeof(char) * m_uMaxStrSize;
	m_nSize += m_nSizeSession;
	//
	m_uAType = 0;
	m_uAId = 0;
	m_ulAPID = 0;
	m_nANbIOs = 0;
	//
	m_cContainer = 0;
	m_cData = 0;
}

isl::CSimulations::~CSimulations()
{
}

unsigned int isl::CSimulations::GetMaxStringSize()
{
	return m_uMaxStrSize;
}

int isl::CSimulations::GetMaxNb()
{
	return m_nMaxNbSims;
}

int isl::CSimulations::GetSizeOneSim()
{
	return m_nSize;
}

unsigned int isl::CSimulations::GetType()
{
	return m_uAType;
}

void isl::CSimulations::SetId(unsigned int uId)
{
	m_uAId = uId;
}

unsigned int isl::CSimulations::GetId()
{
	return m_uAId;
}

void isl::CSimulations::SetPID(unsigned long ulPID)
{
	m_ulAPID = ulPID;
}

unsigned long isl::CSimulations::GetPID()
{
	return m_ulAPID;
}

int isl::CSimulations::GetNbIOs()
{
	return m_nANbIOs;
}

std::string isl::CSimulations::GetUUID()
{
	return m_sAUUID;
}

void isl::CSimulations::SetName(const std::string & sName)
{
	m_sAName = sName;
}

std::string isl::CSimulations::GetName()
{
	return m_sAName;
}

std::string isl::CSimulations::GetFile()
{
	return m_sAFile;
}

void isl::CSimulations::SetSessionId(const std::string & sSessionId)
{
	m_sASessionId = sSessionId;
}

std::string isl::CSimulations::GetSessionId()
{
	return m_sASessionId;
}

void * isl::CSimulations::Add()
{
	if (m_cData == 0) {
		if (Connect() == false) {
			return 0;
		}
	}
	m_cContainer->Lock();
	void * pSim = m_cData;
	for (int i = 0; i < m_nMaxNbSims; i++) {
		if (*(unsigned int *)pSim == 0) {
			// Return the pointer to the part of the SHM related to this simulation
			void * pASimData = pSim;
			// ISL id of the model
			*(unsigned int *)pSim = m_uAId;
			pSim = (char *)pSim + m_nSizeId;
			// PID
			*(unsigned long *)pSim = m_ulAPID;
			pSim = (char *)pSim + m_nSizePID;
			// Model name
			int nSize = m_sAName.size();
			*(int *)pSim = nSize;
			pSim = (char *)pSim + m_nSizeSizeName;
			if (nSize != 0) {
				memcpy(pSim, m_sAName.c_str(), nSize);
				((char *)pSim)[nSize] = '\0';
			}
			pSim = (char *)pSim + m_nSizeName;
			// Session id
			nSize = m_sASessionId.size();
			*(int *)pSim = nSize;
			pSim = (char *)pSim + m_nSizeSizeSession;
			if (nSize != 0) {
				memcpy(pSim, m_sASessionId.c_str(), nSize);
				((char *)pSim)[nSize] = '\0';
			}
			pSim = (char *)pSim + m_nSizeSession;
			//
			m_cContainer->Unlock();
			return pASimData;
		}
		pSim = (char *)pSim + m_nSize;
	}
	m_cContainer->Unlock();
	return 0;
}

bool isl::CSimulations::Remove(void * pASimData)
{
	if (m_cData == NULL) {
		return false;
	}
	if (pASimData == NULL) {
		return false;
	}
	m_cContainer->Lock();
	void * pSim = pASimData;
	// ISL id of the model
	*(unsigned int *)pSim = 0;
	pSim = (char *)pSim + m_nSizeId;
	// PID
	*(unsigned long *)pSim = 0;
	pSim = (char *)pSim + m_nSizePID;
	// Model name
	*(int *)pSim = 0;
	pSim = (char *)pSim + m_nSizeSizeName;
	*(char *)pSim = '\0';
	pSim = (char *)pSim + m_nSizeName;
	// Session name
	*(int *)pSim = 0;
	pSim = (char *)pSim + m_nSizeSizeSession;
	*(char *)pSim = '\0';
	//
	m_cContainer->Unlock();
	return true;
}

bool isl::CSimulations::Get(int nInd)
{
	if (m_cData == NULL) {
		if (Connect() == false) {
			return false;
		}
	}
	if ((nInd < 0) || (nInd >= m_nMaxNbSims)) {
		return false;
	}
	m_cContainer->Lock();
	void * pSim = (char *)m_cData + nInd * m_nSize;
	// ISL id of the model
	m_uAId = *(unsigned int *)pSim;
	pSim = (char *)pSim + m_nSizeId;
	// PID
	m_ulAPID = *(unsigned long *)pSim;
	pSim = (char *)pSim + m_nSizePID;
	// Model name
	pSim = (char *)pSim + m_nSizeSizeName;
	m_sAName = std::string((char *)pSim);
	pSim = (char *)pSim + m_nSizeName;
	// Session id
	pSim = (char *)pSim + m_nSizeSizeSession;
	m_sASessionId = std::string((char *)pSim);
	//
	m_cContainer->Unlock();
	//
	// Try to connect on the connector SHM
	std::string sSHM(boost::str(boost::format(SHM_MODEL_KEY_ID) % m_sASessionId % m_uAId));
	bool bIsGlobalIPC = CAppSettings().IsGlobalIPC();
	CSHM * cMem = 0;
	if (CAppSettings().IsISLCompatible()) {
		cMem = new CSHM(sSHM, "qipc_sharedmemory_", bIsGlobalIPC);
	}
	else {
		cMem = new CSHM(sSHM, bIsGlobalIPC);
	}
	if (cMem->Attach(CSHM::READONLY)) {
		CSHMConnect cSimData(cMem->Data(), NULL);
		m_uAType = cSimData.GetType();
		m_nANbIOs = cSimData.GetNbData();
		m_sAUUID = cSimData.GetUUID();
		m_sAFile = cSimData.GetFile();
	}
	cMem->Detach();
	delete cMem;
	//
	return true;
}

bool isl::CSimulations::Connect()
{
	if (m_cData != 0) {
		return true;
	}
	// ISL shared memory
	std::string sSHMId(boost::str(boost::format(SHM_ISL_KEY_ID) % c_sKeyGeneralId));
	bool bIsGlobalIPC = CAppSettings().IsGlobalIPC();
	CSHM * cISLMem = 0;
	if (CAppSettings().IsISLCompatible()) {
		cISLMem = new CSHM(sSHMId, "qipc_sharedmemory_", bIsGlobalIPC);
	}
	else {
		cISLMem = new CSHM(sSHMId, bIsGlobalIPC);
	}
	int nISLSize = m_nMaxNbSims * m_nSize;
	// Try to create
	bool bISLCreated = cISLMem->Create(nISLSize, CSHM::READWRITE);
	if (bISLCreated == false) {
		if (cISLMem->GetStatus() == CSHM::ALREADYEXISTS) {
			// Try to attach
			if (cISLMem->Attach(CSHM::READWRITE) == false) {
				AppLogError(ISLSIMS_CONNECT_FAILEDTOATTACHSHM,
					"Simulations management: failed to attach shared memory '%s': %d-%d",
					sSHMId.c_str(), cISLMem->GetStatus(), cISLMem->GetError());
				delete cISLMem;
				return false;
			}
			AppLogInfo(ISLSIMS_CONNECT_CONNECTED,
				"Simulations management: Connected to the shared memory '%s'.",
				sSHMId.c_str());
		}
		else {
			AppLogError(ISLSIMS_CONNECT_FAILEDTOCREATESHM,
				"Simulations management: failed to create shared memory '%s': %d-%d",
				sSHMId.c_str(), cISLMem->GetStatus(), cISLMem->GetError());
			delete cISLMem;
			return false;
		}
	}
	else {
		AppLogInfo(ISLSIMS_CONNECT_CREATED,
			"Simulations management: Shared memory '%s' created.", sSHMId.c_str());
	}
	// We are connected to the ISL shared memory
	m_cContainer = cISLMem;
	if (cISLMem->Data() == 0) {
		AppLogError(ISLSIMS_CONNECT_FAILEDTOGETDATA,
			"Simulations management: failed to get access to the allocated space.");
		return false;
	}
	m_cData = (CSHMConnect *)cISLMem->Data();
	if (bISLCreated) {
		// Initialize
		cISLMem->Lock();
		Initialize();
		cISLMem->Unlock();
	}
	return true;
}

bool isl::CSimulations::Disconnect()
{
	bool bRet = true;
	if (m_cContainer != NULL) {
		// The status is not changed even if it returns false
		// If it returns false, it means that the shared memory is still used by another process
		AppLogInfo(ISLCONNECT_DISCONNECT_DETACHSHM,
			"Simulations management: detach shared memory...");
		bRet = m_cContainer->Detach();
		delete m_cContainer;
	}
	m_cContainer = 0;
	m_cData = 0;
	return bRet;
}

bool isl::CSimulations::Initialize()
{
	if (m_cData == 0) {
		return false;
	}
	void * pSim = m_cData;
	for (int i = 0; i < m_nMaxNbSims; i++) {
		// ISL id of the model
		*(unsigned int *)pSim = 0;
		pSim = (char *)pSim + m_nSizeId;
		// PID
		*(unsigned long *)pSim = 0;
		pSim = (char *)pSim + m_nSizePID;
		// Model name
		*(int *)pSim = 0;
		pSim = (char *)pSim + m_nSizeSizeName;
		*(char *)pSim = '\0';
		pSim = (char *)pSim + m_nSizeName;
		// Session name
		*(int *)pSim = 0;
		pSim = (char *)pSim + m_nSizeSizeSession;
		*(char *)pSim = '\0';
		pSim = (char *)pSim + m_nSizeSession;
	}
	return true;
}

isl::CSimulations * isl::CSimulations::Instance()
{
	if (m_cInstance == NULL) {
		m_cInstance = new CSimulations();
	}
	return m_cInstance;
}

void isl::CSimulations::Close()
{
	if (m_cInstance != NULL) {
		m_cInstance->Disconnect();
		delete m_cInstance;
		m_cInstance = NULL;
	}
}

int isl::CSimulations::GetMaxNbSimulations()
{
	return CAppSettings().GetMaxNbRunSims();
}
