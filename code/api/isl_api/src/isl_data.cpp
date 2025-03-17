/*
 *     Name: isl_data.h
 *
 *     Description: ISL API Data class.
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
#include <isl_thread.h>
#include <isl_sem.h>
#include <isl_shm.h>

#include "isl_errorcodes.h"
#include "isl_settings.h"
#include "isl_shm_data.h"
#include "isl_data.h"


/*
 *     Macros and constants definition
 */

#define SHM_KEY_ID		"_isl_shm_ses%1%_sig%2%"
#define SEM_WR_KEY_ID	"_isl_sem_ses%1%_swr%2%"
#define SEM_RD_KEY_ID	"_isl_sem_ses%1%_srd%2%"


/*
 *     Classes definition
 */

isl::CData::CData(CConnect * cParent, const std::string & sId)
	: CVariable(cParent, sId)
{
	m_cData = NULL;
	m_cContainer = NULL;
	m_cWriterListen = NULL;
	m_cReaderListen = NULL;
	m_usMaxFifoDepth = CAppSettings().GetMaxFIFODepth();
	m_usFifoDepth = m_usMaxFifoDepth;
	m_nMaxNbReaders = CAppSettings().GetMaxNbReaders();
	m_bManager = false;
	m_bIsConnected = false;
	m_bIsViewer = false;
	m_dStepTolerance = CAppSettings().GetStepTolerance();
	m_dOriginalStep = -1.0;
	m_dTmpStep = 0.0;
	// Additional parameters initialization is done in InitOnCheck
}

isl::CData::~CData()
{
	if (m_bIsConnected) {
		if (m_bIsViewer == false) {
			Disconnect();
		}
		else {
			DisconnectAsViewer();
		}
	}
	m_cParent = NULL;
}

void isl::CData::SetFifoDepth(unsigned short usDepth)
{
	if ((usDepth > 0) && (usDepth <= m_usMaxFifoDepth)) {
		m_usFifoDepth = usDepth;
	}
}

unsigned short isl::CData::GetFifoDepth()
{
	return m_usFifoDepth;
}

unsigned short isl::CData::GetMaxFifoDepth()
{
	return m_usMaxFifoDepth;
}

bool isl::CData::IsFifoFull()
{
	if (m_cData != NULL) {
		return ((CSHMData *)m_cData)->IsFifoFull();
	}
	return 0;
}

bool isl::CData::IsFifoEmpty()
{
	if (m_cData != NULL) {
		return ((CSHMData *)m_cData)->IsFifoEmpty();
	}
	return 0;
}

int isl::CData::GetReaders()
{
	if (m_cData != NULL) {
		return ((CSHMData *)m_cData)->GetReaders();
	}
	return 0;
}

unsigned char isl::CData::GetIndWriter()
{
	if (m_cData != NULL) {
		return ((CSHMData *)m_cData)->GetIndWriter();
	}
	return 0;
}

unsigned char isl::CData::GetIndReader(int i)
{
	if (m_cData != NULL) {
		return ((CSHMData *)m_cData)->GetIndReader(i);
	}
	return 0;
}

int isl::CData::GetMaxReaders()
{
	return m_nMaxNbReaders;
}

bool isl::CData::Initialize(double dTime)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bRet = false;
	m_cContainer->Lock();
	bRet = ((CSHMData *)m_cData)->InitializeData(dTime);
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::SetData(void * pData, double dTime, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bListen = false;
	m_cContainer->Lock();
	bool bRet = ((CSHMData *)m_cData)->SetData(pData, dTime, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// Wait until the FIFO is not full anymore
	while (bListen == true) {
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetData locked on t=%gs for '%s'", dTime, m_sId.c_str());
#endif
		if (m_cWriterListen->Acquire() == false) {
			return false;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetData unlocked on t=%gs for '%s'",
			dTime, m_sId.c_str());
#endif
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->SetData(pData, dTime, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If readers are waiting then unlock them
	m_cContainer->Lock();
	int nListeners = ((CSHMData *)m_cData)->GetReaderListen();
#if 0 // TODO: verify the code below
	int nListenersToSet = nListeners;
#endif
	int nReaders = ((CSHMData *)m_cData)->GetReaders();
	if (nReaders > 1) {
		// More than one reader, then we should release the semaphore
		// for all readers even if they are not locked
#if 0 // TODO: verify the code below
		if (((nListeners + nReaders > 0) && (nListeners < 0))
			|| ((nListeners - nReaders < 0) && (nListeners > 0))) {
			nListeners = nReaders;
		}
#else
		if ((nListeners - nReaders < 0) && (nListeners > 0)) {
			nListeners = nReaders;
		}
#endif
	}
	if (nListeners > 0) {
		m_cReaderListen->Release(nListeners);
#if 0 // TODO: verify the code below
		((CSHMData *)m_cData)->SetReaderListen(nListenersToSet - nListeners);
#else
		((CSHMData *)m_cData)->SetReaderListen(0);
#endif
	}
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::SetData(void * pData, double dTime, double dStep, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bListen = false;
	m_cContainer->Lock();
	bool bRet = ((CSHMData *)m_cData)->SetData(pData, dTime, dStep, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// Wait until the FIFO is not full anymore
	while (bListen == true) {
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetData(step) locked on t=%gs for '%s'",
			dTime, m_sId.c_str());
#endif
		if (m_cWriterListen->Acquire() == false) {
			return false;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetData(step) unlocked on t=%gs for '%s'",
			dTime, m_sId.c_str());
#endif
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->SetData(pData, dTime, dStep, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If readers are waiting then unlock them
	m_cContainer->Lock();
	int nListeners = ((CSHMData *)m_cData)->GetReaderListen();
#if 0 // TODO: verify the code below
	int nListenersToSet = nListeners;
#endif
	int nReaders = ((CSHMData *)m_cData)->GetReaders();
	if (nReaders > 1) {
		// More than one reader, then we should release the semaphore
		// for all readers even if they are not locked
#if 0 // TODO: verify the code below
		if (((nListeners + nReaders > 0) && (nListeners < 0))
			|| ((nListeners - nReaders < 0) && (nListeners > 0))) {
			nListeners = nReaders;
		}
#else
		if ((nListeners - nReaders < 0) && (nListeners > 0)) {
			nListeners = nReaders;
		}
#endif
	}
	if (nListeners > 0) {
		m_cReaderListen->Release(nListeners);
#if 0 // TODO: verify the code below
		((CSHMData *)m_cData)->SetReaderListen(nListenersToSet - nListeners);
#else
		((CSHMData *)m_cData)->SetReaderListen(0);
#endif
	}
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::SetLastData(double dTime, double dStep, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bListen = false;
	m_cContainer->Lock();
	bool bRet = ((CSHMData *)m_cData)->SetLastData(dTime, dStep, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// Wait until the FIFO is not full anymore
	while (bListen == true) {
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetLastData(step) locked on t=%gs for '%s'",
			dTime, m_sId.c_str());
#endif
		if (m_cWriterListen->Acquire() == false) {
			return false;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetLastData(step) unlocked on t=%gs for '%s'",
			dTime, m_sId.c_str());
#endif
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->SetLastData(dTime, dStep, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If readers are waiting then unlock them
	m_cContainer->Lock();
	int nListeners = ((CSHMData *)m_cData)->GetReaderListen();
#if 0 // TODO: verify the code below
	int nListenersToSet = nListeners;
#endif
	int nReaders = ((CSHMData *)m_cData)->GetReaders();
	if (nReaders > 1) {
		// More than one reader, then we should release the semaphore
		// for all readers even if they are not locked
#if 0 // TODO: verify the code below
		if (((nListeners + nReaders > 0) && (nListeners < 0))
			|| ((nListeners - nReaders < 0) && (nListeners > 0))) {
			nListeners = nReaders;
		}
#else
		if ((nListeners - nReaders < 0) && (nListeners > 0)) {
			nListeners = nReaders;
		}
#endif
	}
	if (nListeners > 0) {
		m_cReaderListen->Release(nListeners);
#if 0 // TODO: verify the code below
		((CSHMData *)m_cData)->SetReaderListen(nListenersToSet - nListeners);
#else
		((CSHMData *)m_cData)->SetReaderListen(0);
#endif
	}
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::GetData(void * pData, double * dTime, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bListen = false;
	m_cContainer->Lock();
	bool bIsFifoFull = ((CSHMData *)m_cData)->IsFifoFullForReader();
	bool bRet = ((CSHMData *)m_cData)->GetData(pData, dTime, &m_dTmpStep, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// The FIFO is empty
	// Wait until we get a new value in the FIFO
	while (bListen == true) {
		if (m_cReaderListen->Acquire() == false) {
			return false;
		}
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		bIsFifoFull = ((CSHMData *)m_cData)->IsFifoFullForReader();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->GetData(pData, dTime, &m_dTmpStep, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If the FIFO is considered as full for this reader
	// then the writer is probably waiting
	if (bIsFifoFull) {
		m_cContainer->Lock();
		int nListeners = ((CSHMData *)m_cData)->GetWriterListen();
		if (nListeners > 0) {
			m_cWriterListen->Release(nListeners);
			((CSHMData *)m_cData)->SetWriterListen(0);
		}
		m_cContainer->Unlock();
	}
	return bRet;
}

bool isl::CData::GetData(void * pData, double * dOutTime, double dInTime, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	double dOutStep = 0.0;
	bool bListen = false;
	m_cContainer->Lock();
	bool bWasFifoFull = ((CSHMData *)m_cData)->IsFifoFullForReader();
	bool bRet = ((CSHMData *)m_cData)->GetData(pData, dOutTime, &dOutStep, dInTime, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// The FIFO is considered as empty
	// Wait until we get a new value in the FIFO
	while (bListen == true) {
		// If the FIFO was considered as full for this reader
		// then the writer is probably waiting
		if (bWasFifoFull) {
			m_cContainer->Lock();
			int nListeners = ((CSHMData *)m_cData)->GetWriterListen();
			if (nListeners > 0) {
				m_cWriterListen->Release(nListeners);
				((CSHMData *)m_cData)->SetWriterListen(0);
			}
			m_cContainer->Unlock();
		}
		// Wait for a new value
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "GetData locked on t=%gs for '%s'",
			dInTime, m_sId.c_str());
#endif
		if (m_cReaderListen->Acquire() == false) {
			return false;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "GetData unlocked on t=%gs for '%s'",
			dInTime, m_sId.c_str());
#endif
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		bWasFifoFull = ((CSHMData *)m_cData)->IsFifoFullForReader();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->GetData(pData, dOutTime, &dOutStep, dInTime, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If the FIFO was considered as full for this reader
	// then the writer is probably waiting
	if (bWasFifoFull) {
		m_cContainer->Lock();
		int nListeners = ((CSHMData *)m_cData)->GetWriterListen();
		if (nListeners > 0) {
			m_cWriterListen->Release(nListeners);
			((CSHMData *)m_cData)->SetWriterListen(0);
		}
		m_cContainer->Unlock();
	}
	// If an event data is received => outtime is now intime
	if (*dOutTime == EVENT_DEF_TIME_VAL) {
		*dOutTime = dInTime;
	}
	// Compute data for the requested time?
#if 0 // TODO: to add
	if (bRet && (m_cCompute != NULL)) {
		double dTolerance = m_cParent->GetStepTolerance();
		double dData = *((double *)pData);
		if ((*dOutTime < dInTime - dOutStep*dTolerance)
			|| (*dOutTime > dInTime + dOutStep*dTolerance)) {
			// Compute data
			int nRet = TTSComputeFcts->TTS_Compute_Get(this, &dData, dInTime);
			if (nRet == COMPUTE_STATUS_SUCCESS) {
				*((double *)pData) = dData;
				*dOutTime = dInTime;
			}
		}
		else {
			// Set point
			TTSComputeFcts->TTS_Compute_Set(this, dData, *dOutTime);
		}
	}
#endif
	return bRet;
}

bool isl::CData::GetDataAndStep(void * pData, double * dTime, double * dStep, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bRet = GetData(pData, dTime, bWait);
	*dStep = m_dTmpStep;
	return bRet;
}

bool isl::CData::SetEventData(void * pData, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	bool bListen = false;
	m_cContainer->Lock();
	bool bRet = ((CSHMData *)m_cData)->SetData(pData, EVENT_DEF_TIME_VAL, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// Wait until the FIFO is not full anymore
	while (bListen == true) {
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetEventData locked on for '%s'", m_sId.c_str());
#endif
		if (m_cWriterListen->Acquire() == false) {
			return false;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLDATA_DEBUG, "SetData unlocked on for '%s'", m_sId.c_str());
#endif
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->SetData(pData, EVENT_DEF_TIME_VAL, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If readers are waiting then unlock them
	m_cContainer->Lock();
	int nListeners = ((CSHMData *)m_cData)->GetReaderListen();
#if 0 // TODO: verify the code below
	int nListenersToSet = nListeners;
#endif
	int nReaders = ((CSHMData *)m_cData)->GetReaders();
	if (nReaders > 1) {
		// More than one reader, then we should release the semaphore
		// for all readers even if they are not locked
#if 0 // TODO: verify the code below
		if (((nListeners + nReaders > 0) && (nListeners < 0))
			|| ((nListeners - nReaders < 0) && (nListeners > 0))) {
			nListeners = nReaders;
		}
#else
		if ((nListeners - nReaders < 0) && (nListeners > 0)) {
			nListeners = nReaders;
		}
#endif
	}
	if (nListeners > 0) {
		m_cReaderListen->Release(nListeners);
#if 0 // TODO: verify the code below
		((CSHMData *)m_cData)->SetReaderListen(nListenersToSet - nListeners);
#else
		((CSHMData *)m_cData)->SetReaderListen(0);
#endif
	}
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::GetEventData(void * pData, bool bWait)
{
	if (IsConnected() == false) {
		return false;
	}
	double dTime = 0.0;
	bool bListen = false;
	m_cContainer->Lock();
	bool bIsFifoFull = ((CSHMData *)m_cData)->IsFifoFullForReader();
	bool bRet = ((CSHMData *)m_cData)->GetData(pData, &dTime, &m_dTmpStep, (bWait ? &bListen : NULL));
	m_cContainer->Unlock();
	// The FIFO is empty
	// Wait until we get a new value in the FIFO
	while (bListen == true) {
		if (m_cReaderListen->Acquire() == false) {
			return false;
		}
		m_cContainer->Lock();
		bool bIsTerminated = ((CSHMData *)m_cData)->IsTerminated();
		m_cContainer->Unlock();
		if (bIsTerminated) {
			// Simulation ended
			m_cParent->SetTerminated();
			return false;
		}
		m_cContainer->Lock();
		bIsFifoFull = ((CSHMData *)m_cData)->IsFifoFullForReader();
		// bListen == true means bWait == true
		bRet = ((CSHMData *)m_cData)->GetData(pData, &dTime, &m_dTmpStep, &bListen);
		// bListen will be set to false if we don't need to wait anymore
		m_cContainer->Unlock();
	}
	// If the FIFO is considered as full for this reader
	// then the writer is probably waiting
	if (bIsFifoFull) {
		m_cContainer->Lock();
		int nListeners = ((CSHMData *)m_cData)->GetWriterListen();
		if (nListeners > 0) {
			m_cWriterListen->Release(nListeners);
			((CSHMData *)m_cData)->SetWriterListen(0);
		}
		m_cContainer->Unlock();
	}
	return bRet;
}

bool isl::CData::SetData(void * pData, double dTime, int nInd)
{
	if (IsConnected() == false) {
		return false;
	}
	m_cContainer->Lock();
	bool bRet = ((CSHMData *)m_cData)->SetData(pData, dTime, nInd);
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::GetData(void * pData, double * dTime, int nInd)
{
	if (IsConnected() == false) {
		return false;
	}
	m_cContainer->Lock();
	bool bRet = ((CSHMData *)m_cData)->GetData(pData, dTime, nInd);
	m_cContainer->Unlock();
	return bRet;
}

bool isl::CData::GetMemData(void * pData, double * dTime, double * dStep, int nInd)
{
	if (IsConnected() == false) {
		return false;
	}
	return ((CSHMData *)m_cData)->GetMemData(pData, dTime, dStep, nInd);
}

bool isl::CData::StoreData(void * pData, double dTime)
{
#if 0 // TODO: to add
	CDatabaseManager * cDM = (CDatabaseManager *)(m_cParent->GetStore());
	if (cDM == NULL) {
		return false;
	}
	QString sData;
	int nSize = m_cVar->GetSize();
	if (nSize <= 0) {
		nSize = 1;
	}
	switch (m_cVar->GetType()->GetType()) {
		case CFMUXDataType::FMUX_DATA_TYPE_BOOLEAN:
		{
			bool bVal = ((bool *)pData)[0];
			sData = (bVal ? "1" : "0");
			for (int i = 1; i < nSize; i++) {
				bVal = ((bool *)pData)[i];
				sData += (bVal ? ",1" : ",0");
			}
			break;
		}
		case CFMUXDataType::FMUX_DATA_TYPE_ENUMERATION:
		case CFMUXDataType::FMUX_DATA_TYPE_INTEGER:
		{
			int nVal = ((int *)pData)[0];
			sData = QString::number(nVal);
			for (int i = 1; i < nSize; i++) {
				nVal = ((int *)pData)[i];
				sData += QString(",%1").arg(nVal);
			}
			break;
		}
		case CFMUXDataType::FMUX_DATA_TYPE_REAL:
		{
			double dVal = ((double *)pData)[0];
			sData = QString::number(dVal);
			for (int i = 1; i < nSize; i++) {
				dVal = ((double *)pData)[i];
				sData += QString(",%1").arg(dVal);
			}
			break;
		}
		case CFMUXDataType::FMUX_DATA_TYPE_STRING:
		{
			char cVal = ((char *)pData)[0];
			sData = cVal;
			for (int i = 1; i < nSize; i++) {
				cVal = ((char *)pData)[i];
				sData += cVal;
			}
			break;
		}
		default:
			return false;
	}
	return cDM->AddData(m_cVar->GetReference(), dTime, sData);
#else
	return true;
#endif
}

int isl::CData::Connect(bool bWait, int nTimeOut)
{
	m_bIsConnected = true;
	m_bIsViewer = false;
	m_bManager = IsOutput();
	if (m_cContainer != 0) {
		AppLogWarning(ISLDATA_ALREADY_CONNECTED,
			"Variable '%s' already connected.", m_sId.c_str());
		return 1; // Already connected
	}
	if (m_sConnectId.empty()) {
		AppLogWarning(ISLDATA_NO_CONNECTID,
			"Variable '%s' without connection identifier.", m_sId.c_str());
		return 2;
	}
	if (m_cParent == 0) {
		AppLogError(ISLDATA_NO_CONNECTOR_LINKED,
			"No connector associated to the variable '%s'.", m_sId.c_str());
		return -1;
	}
	std::string sSession = m_cParent->GetSessionId();
	std::string sSHM(boost::str(boost::format(SHM_KEY_ID) % sSession % m_sConnectId));
	bool bIsGlobalIPC = CAppSettings().IsGlobalIPC();
	CSHM * cMem = 0;
	if (CAppSettings().IsISLCompatible()) {
		cMem = new CSHM(sSHM, "qipc_sharedmemory_", bIsGlobalIPC);
	}
	else {
		cMem = new CSHM(sSHM, bIsGlobalIPC);
	}
	if (m_bManager) {
		AppLogInfo(ISLDATA_CONNECT_CREATESHM, "Variable '%s': creating the shared memory '%s'...",
			m_sId.c_str(), sSHM.c_str());
		// Compute the size
		int nSize = CSHMData::GetSizeOf(this);
		if (nSize <= 0) {
			AppLogError(ISLDATA_WRONG_SHMSIZE, "Variable '%s': wrong shared memory size: %d.",
				m_sId.c_str(), nSize);
			delete cMem;
			return -2;
		}
		// Create the SHM
		if (cMem->Create(nSize, CSHM::READWRITE) == false) {
			AppLogError(ISLDATA_FAILED_CREATESHM,
				"Variable '%s': Failed to create the shared memory. %d-%d",
				m_sId.c_str(), cMem->GetStatus(), cMem->GetError());
			delete cMem;
			return -3;
		}
	}
	else {
		AppLogInfo(ISLDATA_CONNECT_CONNECTSHM, "Variable '%s': connecting to the shared memory '%s'...",
			m_sId.c_str(), sSHM.c_str());
		// Connect to the SHM
		bool bContinueToListen = true;
		int nTime = 0;
		while (bContinueToListen) {
			if (cMem->Attach(CSHM::READWRITE) == false) {
				CSHM::tStatus eState = cMem->GetStatus();
				if ((eState != CSHM::NOTFOUND) && (eState != CSHM::SUCCESS)) {
					AppLogError(ISLDATA_FAILED_CONNECTSHM,
						"Variable '%s': Failed to connect to the shared memory. %d-%d",
						m_sId.c_str(), eState, cMem->GetError());
					delete cMem;
					return -4;
				}
				if (bWait == false) {
					return 3;
				}
				CThread::Sleep(500); // Wait for 500ms before trying again
				if (nTimeOut > 0) {
					nTime += 500;
					if (nTime >= nTimeOut * 1000) {
						delete cMem;
						return 3;
					}
				}
			}
			else {
				// Found
				bContinueToListen = false;
			}
		}
	}
	m_cContainer = cMem;
	if (cMem->Data() == NULL) {
		AppLogError(ISLDATA_NOVALIDSHMSPACE,
			"Variable '%s': Not a valid memory allocated space.", m_sId.c_str());
		return -5;
	}
	CSHMData * cData = new CSHMData(cMem->Data(), this);
	if (m_bManager) {
		cMem->Lock();
		if (cData->Initialize() == false) {
			AppLogError(ISLDATA_FAILED_INITSHM,
				"Variable '%s': Failed to initialize the shared memory.", m_sId.c_str());
			cMem->Unlock();
			return -6;
		}
		cMem->Unlock();
	}
	else {
		cMem->Lock();
		cData->SetReader();
		cMem->Unlock();
	}
	m_cData = cData;
	// Create or open the semaphores
	std::string sSem(boost::str(boost::format(SEM_WR_KEY_ID) % sSession % m_sConnectId));
	CSem * cSem = 0;
	if (CAppSettings().IsISLCompatible()) {
		cSem = new CSem(sSem, "qipc_systemsem_", 0, CSem::CREATE, bIsGlobalIPC);
	}
	else {
		cSem = new CSem(sSem, 0, CSem::CREATE, bIsGlobalIPC);
	}
	// If the semaphore is already created, it will connect in open mode.
	cSem->SetTimeout(m_nSyncTimeout);
	m_cWriterListen = cSem;
	sSem = boost::str(boost::format(SEM_RD_KEY_ID) % sSession % m_sConnectId);
	cSem = 0;
	if (CAppSettings().IsISLCompatible()) {
		cSem = new CSem(sSem, "qipc_systemsem_", 0, CSem::CREATE, bIsGlobalIPC);
	}
	else {
		cSem = new CSem(sSem, 0, CSem::CREATE, bIsGlobalIPC);
	}
	cSem->SetTimeout(m_nSyncTimeout);
	// If the semaphore is already created, it will connect in open mode.
	m_cReaderListen = cSem;
	// Compute method?
#if 0 // TODO: to add
	if (GetComputeType() != COMPUTE_TYPE_NOTYPE) {
		if (TTSComputeFcts->GetStatus() == CTTSComputeFcts::FCT_STATUS_SUCCESS) {
			TTSComputeFcts->TTS_Compute_Initialize(this);
		}
		else {
			qWarning("Failed to load the compute library (%d).", TTSComputeFcts->GetStatus());
		}
	}
#endif
	AppLogInfo(ISLDATA_CONNECT_CONNECTED, "Variable '%s': connected to the ISL session.", m_sId.c_str());
	return 0;
}

bool isl::CData::Disconnect()
{
	// Disconnect and delete the semaphores
	if (m_cReaderListen != NULL) {
		m_cContainer->Lock();
		int nListeners = ((CSHMData *)m_cData)->GetReaderListen();
		((CSHMData *)m_cData)->SetTerminated();
		if (nListeners > 0) { // Release all listeners
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLDATA_DEBUG, "READERLISTEN unlock all for %s: %d",
				m_sId.c_str(), nListeners);
#endif
			m_cReaderListen->Release(nListeners);
			((CSHMData *)m_cData)->SetReaderListen(0);
		}
		m_cContainer->Unlock();
		// Wait few milliseconds before deleting
		CThread::Sleep(200);
		delete m_cReaderListen;
	}
	m_cReaderListen = NULL;
	if (m_cWriterListen != NULL) {
		m_cContainer->Lock();
		int nListeners = ((CSHMData *)m_cData)->GetWriterListen();
		((CSHMData *)m_cData)->SetTerminated();
		if (nListeners > 0) { // Release all listeners
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLDATA_DEBUG, "WRITERLISTEN unlock all for %s: %d",
				m_sId.c_str(), nListeners);
#endif
			m_cWriterListen->Release(nListeners);
			((CSHMData *)m_cData)->SetWriterListen(0);
		}
		m_cContainer->Unlock();
		// Wait few milliseconds before deleting
		CThread::Sleep(200);
		delete m_cWriterListen;
	}
	m_cWriterListen = NULL;
	bool bRet = true;
	if (m_cContainer != NULL) {
		// The status is not changed even if it returns false
		// If it returns false, it means that the shared memory is still locked by another process
		AppLogInfo(ISLDATA_DETACHINGSHM, "Variable %s: detaching shared memory...", m_sId.c_str());
		bRet = m_cContainer->Detach();
		delete m_cContainer;
	}
	m_cContainer = NULL;
	if (m_cData != NULL) {
		delete (CSHMData *)m_cData;
	}
	m_cData = NULL;
	return bRet;
}

bool isl::CData::ConnectAsViewer(bool bWait)
{
	m_bIsConnected = true;
	m_bIsViewer = true;
	if (m_cContainer != NULL) {
		AppLogWarning(ISLDATA_ALREADY_CONNECTED,
			"Variable %s already connected.", m_sId.c_str());
		return false; // Already connected
	}
	if (m_sConnectId.empty()) {
		AppLogWarning(ISLDATA_NO_CONNECTID,
			"Variable %s without connection identifier.", m_sId.c_str());
		return false;
	}
	if (m_cParent == NULL) {
		AppLogError(ISLDATA_NO_CONNECTOR_LINKED,
			"No connector associated to the variable %s.", m_sId.c_str());
		return false;
	}
	std::string sSession = m_cParent->GetSessionId();
	std::string sSHM(boost::str(boost::format(SHM_KEY_ID) % sSession % m_sConnectId));
	bool bIsGlobalIPC = CAppSettings().IsGlobalIPC();
	CSHM * cMem = 0;
	if (CAppSettings().IsISLCompatible()) {
		cMem = new CSHM(sSHM, "qipc_sharedmemory_", bIsGlobalIPC);
	}
	else {
		cMem = new CSHM(sSHM, bIsGlobalIPC);
	}
	// Connect to the SHM
	AppLogInfo(ISLDATA_CONNECT_CONNECTSHM,
		"Variable '%s' (viewer mode): connecting to the shared memory '%s'...",
		m_sId.c_str(), sSHM.c_str());
	bool bContinueToListen = true;
	while (bContinueToListen) {
		if (cMem->Attach(CSHM::READONLY) == false) {
			CSHM::tStatus eState = cMem->GetStatus();
			if ((bWait == false) || ((eState != CSHM::NOTFOUND) && (eState != CSHM::SUCCESS))) {
				AppLogError(ISLDATA_FAILED_CONNECTSHM,
					"Variable %s: Viewer cannot connect to the shared memory. %d-%d",
					m_sId.c_str(), cMem->GetStatus(), cMem->GetError());
				delete cMem;
				return false;
			}
			CThread::Sleep(500); // Wait for 500ms before trying again
		}
		else {
			// Found
			bContinueToListen = false;
		}
	}
	m_cContainer = cMem;
	if (cMem->Data() == 0) {
		AppLogError(ISLDATA_NOVALIDSHMSPACE,
			"Variable %s: Not a valid memory allocated space.", m_sId.c_str());
		return false;
	}
	CSHMData * cData = new CSHMData(cMem->Data(), this);
	m_cData = cData;
	AppLogInfo(ISLDATA_CONNECT_CONNECTED,
		"Variable '%s' (viewer mode): connected to the ISL session.", m_sId.c_str());
	return true;
}

bool isl::CData::DisconnectAsViewer()
{
	bool bRet = true;
	if (m_cContainer != NULL) {
		// The status is not changed even if it returns false
		// If it returns false, it means that the shared memory is still locked by another process
		AppLogInfo(ISLDATA_DETACHINGSHM, "Variable %s: detaching shared memory...", m_sId.c_str());
		bRet = m_cContainer->Detach();
		delete m_cContainer;
	}
	m_cContainer = NULL;
	if (m_cData != NULL) {
		delete (CSHMData *)m_cData;
	}
	m_cData = NULL;
	return bRet;
}

bool isl::CData::IsConnected()
{
	return (m_cContainer != 0) && (m_cData != 0);
}

int isl::CData::GetComputeType()
{
#if 0 // TODO: to add
	if (m_cVar != NULL) {
		return m_cVar->GetComputeType();
	}
	return COMPUTE_TYPE_NOTYPE;
#else
	return 0;
#endif
}

void isl::CData::SetCompute(void * cCompute)
{
#if 0 // TODO: to add
	m_cCompute = cCompute;
#endif
}

void * isl::CData::GetCompute()
{
#if 0 // TODO: to add
	return m_cCompute;
#else
	return 0;
#endif
}

double isl::CData::GetOriginalStep()
{
	return m_dOriginalStep;
}

double isl::CData::GetStepTolerance()
{
	return m_dStepTolerance;
}

void isl::CData::InitOnCheck()
{
	AppLogDebug(2, ISLDATA_DEBUG, "Variable '%s': data parameters initialization.", m_sId.c_str());
	m_bManager = IsOutput(); // Initialize the SHM only if it is an output
	m_dOriginalStep = m_dStepSize;
	if (m_cParent != NULL) {
		m_dStepTolerance = m_cParent->GetStepTolerance();
		if (m_dOriginalStep <= 0.0) {
			m_dOriginalStep = m_cParent->GetStepSize();
		}
	}
	m_dTmpStep = 0.0;
}
