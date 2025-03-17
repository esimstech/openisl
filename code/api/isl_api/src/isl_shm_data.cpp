/*
 *     Name: isl_shm_data.cpp
 *
 *     Description: ISL API SHM Data class.
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

#include <math.h>
#include <isl_log.h>
#include "isl_api.h"
#include "isl_errorcodes.h"
#include "isl_settings.h"
#include "isl_shm_data.h"
#ifndef WIN32
#include <limits.h>
#endif


/*
 *     Macros and constants definition
 */

#define MAX_FIFO_DEPTH USHRT_MAX


/*
 *     Classes definition
 */

int isl::CSHMData::GetSizeOf(CData * cData)
{
	if (cData == NULL) {
		return 0;
	}
	if (cData->GetParent() == NULL) {
		return 0;
	}
	int nSize = sizeof(unsigned int); // m_uId
	//
	nSize += sizeof(int); // m_nSizeName
	nSize += sizeof(char) * CAppSettings().GetMaxSHMStringSize(); // m_sName
	//
	nSize += sizeof(CDataType::tType); // m_eType
	nSize += sizeof(int); // m_nSizeType
	nSize += sizeof(int); // m_nSize
	//
	nSize += sizeof(unsigned short); // m_usIndWrite
	nSize += sizeof(int); // m_nReaders
	nSize += sizeof(unsigned short) * cData->GetMaxReaders(); // m_usIndReads
	//
	nSize += sizeof(int); // m_nReaderListen
	nSize += sizeof(int); // m_nWriterListen
	//
	nSize += sizeof(bool); // m_bIsTerminated
	//
	nSize += sizeof(unsigned short); // m_usFifoDepth
	nSize += sizeof(double) * (int )(cData->GetMaxFifoDepth()); // m_dTimes
	nSize += sizeof(double) * (int)(cData->GetMaxFifoDepth()); // m_dSteps
	//
	int nSizeVar = cData->GetType()->GetSizeInBytes();
	if (nSizeVar <= 0) {
		return 0;
	}
	nSize += nSizeVar * (int )(cData->GetMaxFifoDepth()); // m_pData
	//
	return nSize;
}

isl::CSHMData::CSHMData(void * pData, CData * cData)
{
	m_uId = NULL;
	m_nSizeName = NULL;
	m_sName = NULL;
	m_cType = NULL;
	m_eType = NULL;
	m_nSizeType = NULL;
	m_nSize = NULL;
	m_usIndWrite = NULL;
	m_nReaders = NULL;
	m_usIndReads = NULL;
	m_nReaderListen = NULL;
	m_nWriterListen = NULL;
	m_bIsTerminated = NULL;
	m_usFifoDepth = NULL;
	m_dTimes = NULL;
	m_dSteps = NULL;
	m_pData = NULL;
	if ((pData != NULL) && (cData != NULL)) {
		m_uId = (unsigned int *)pData;
		char * pNext = (char *)pData + sizeof(unsigned int);
		//
		m_nSizeName = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_sName = (char *)pNext;
		pNext = pNext + (sizeof(char) * CAppSettings().GetMaxSHMStringSize());
		//
		m_eType = (CDataType::tType *)pNext;
		pNext = pNext + sizeof(CDataType::tType);
		m_nSizeType = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_nSize = (int *)pNext;
		pNext = pNext + sizeof(int);
		//
		m_usIndWrite = (unsigned short *)pNext;
		pNext = pNext + sizeof(unsigned short);
		m_nReaders = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_usIndReads = (unsigned short *)pNext;
		pNext = pNext + (sizeof(unsigned short) * cData->GetMaxReaders());
		//
		m_nReaderListen = (int *)pNext;
		pNext = pNext + sizeof(int);
		m_nWriterListen = (int *)pNext;
		pNext = pNext + sizeof(int);
		//
		m_bIsTerminated = (bool *)pNext;
		pNext = pNext + sizeof(bool);
		//
		m_usFifoDepth = (unsigned short *)pNext;
		pNext = pNext + sizeof(unsigned short);
		m_dTimes = (double *)pNext;
		pNext = pNext + (sizeof(double) * (int )(cData->GetMaxFifoDepth()));
		m_dSteps = (double *)pNext;
		pNext = pNext + (sizeof(double) * (int)(cData->GetMaxFifoDepth()));
		m_pData = (void *)pNext;
	}
	m_cParent = cData;
	m_nReaderInd = -1;
	m_dStepTolerance = m_cParent->GetStepTolerance();
	m_dOriginalStep = m_cParent->GetOriginalStep();
}

isl::CSHMData::~CSHMData()
{
	m_uId = NULL;
	m_nSizeName = NULL;
	m_sName = NULL;
	m_cType = NULL;
	m_eType = NULL;
	m_nSizeType = NULL;
	m_nSize = NULL;
	m_usIndWrite = NULL;
	m_nReaders = NULL;
	m_usIndReads = NULL;
	m_nReaderListen = NULL;
	m_nWriterListen = NULL;
	m_bIsTerminated = NULL;
	m_usFifoDepth = NULL;
	m_dTimes = NULL;
	m_dSteps = NULL;
	m_pData = NULL;
	m_cParent = NULL;
}

bool isl::CSHMData::Initialize()
{
	if (m_cParent == NULL) {
		AppLogError(ISLSHMDATA_NO_CONNECTOR_LINKED, "No ISL data assaociated to the SHM memory.");
		return false;
	}
	if (m_uId == NULL) { // Test the first element
		AppLogError(ISLSHMDATA_NOT_ALLOCATED, "No memory allocated to the variable %s",
			m_cParent->GetId().c_str());
		return false;
	}
	*m_uId = 0; // Unused for now, but is a reference number in ISL
	//
	int nSize = (int )(m_cParent->GetId().size());
	*m_nSizeName = nSize;
	if (nSize >= 0) {
		memcpy(m_sName, m_cParent->GetId().c_str(), nSize);
		m_sName[nSize] = '\0';
	}
	//
	m_cType = m_cParent->GetType();
	*m_eType = m_cParent->GetTypeId();
	*m_nSizeType = m_cParent->GetType()->GetSizeOf();
	int nSizeVar = m_cParent->GetType()->GetSize();
	if (nSizeVar <= 0) { // Not useful as already checked in the variable setup
		nSizeVar = 1;
	}
	*m_nSize = nSizeVar;
	*m_usIndWrite = 0;
	*m_nReaders = 0;
	int nMaxReaders = m_cParent->GetMaxReaders();
	for (int i = 0; i < nMaxReaders; i++) {
		m_usIndReads[i] = 0;
	}
	*m_nReaderListen = 0;
	*m_nWriterListen = 0;
	*m_bIsTerminated = false;
	unsigned short usDepth = m_cParent->GetFifoDepth();
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "FIFO depth initialized from parent: %u", usDepth);
#endif
	*m_usFifoDepth = usDepth;
	for (int i = 0; i < (int )usDepth; i++) {
		m_dTimes[i] = 0.0;
		m_dSteps[i] = m_dOriginalStep;
	}
	return true;
}

bool isl::CSHMData::InitializeData(double dTime)
{
	if (m_cParent == 0 || m_cType == 0) {
		AppLogError(ISLSHMDATA_NO_CONNECTOR_LINKED, "No ISL data assaociated to the SHM memory.");
		return false;
	}
	void * pValue = m_cType->GetInitial();
	if (pValue == 0) {
		AppLogWarning(ISLSHMDATA_NO_INITIAL_VALUE, "No initial value available for the variable %s.",
			m_cParent->GetId().c_str());
		return false;
	}
	unsigned short usDepth = *m_usFifoDepth;
	int nSize = *m_nSize;
	int nSizeType = *m_nSizeType;
	int nDataSize = nSizeType * nSize;
	for (int i = 0; i < (int )usDepth; i++) {
		m_dTimes[i] = dTime;
		void * pData = (char *)m_pData + i*nDataSize;
		MemCopy(pData, pValue, nDataSize, true);
	}
	return true;
}

unsigned int isl::CSHMData::GetId()
{
	return *m_uId;
}

std::string isl::CSHMData::GetName()
{
	return std::string(m_sName);
}

isl::CDataType::tType isl::CSHMData::GetType()
{
	return *m_eType;
}

int isl::CSHMData::GetSizeType()
{
	return *m_nSizeType;
}

int isl::CSHMData::GetSize()
{
	return *m_nSize;
}

unsigned short isl::CSHMData::GetFifoDepth()
{
	return *m_usFifoDepth;
}

unsigned short isl::CSHMData::GetIndWriter()
{
	return *m_usIndWrite;
}

unsigned short isl::CSHMData::GetIndReader(int i)
{
	return m_usIndReads[i];
}

void isl::CSHMData::SetReader()
{
	if (m_nReaderInd == -1) {
		m_nReaderInd = *m_nReaders;
		*m_nReaders = m_nReaderInd + 1;
	}
}

bool isl::CSHMData::IsReader()
{
	return m_nReaderInd != -1;
}

int isl::CSHMData::GetReaderListen()
{
	return *m_nReaderListen;
}

int isl::CSHMData::GetReaders()
{
	return *m_nReaders;
}

void isl::CSHMData::SetReaderListen(int nVal)
{
	*m_nReaderListen = nVal;
}

int isl::CSHMData::GetWriterListen()
{
	return *m_nWriterListen;
}

void isl::CSHMData::SetWriterListen(int nVal)
{
	*m_nWriterListen = nVal;
}

void isl::CSHMData::SetTerminated()
{
	*m_bIsTerminated = true;
}

bool isl::CSHMData::IsTerminated()
{
	return *m_bIsTerminated;
}

bool isl::CSHMData::IsFifoFullForReader()
{
	unsigned short usWrite = *m_usIndWrite;
	unsigned short usDepth = *m_usFifoDepth;
	unsigned short usRead = m_usIndReads[m_nReaderInd];
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "IsFifoFullForReader[%d]? W:%d - R:%d ? %d",
		m_nReaderInd, usWrite, usRead, usDepth);
#endif
	int nNbElts = (int)usWrite - (int)usRead;
	if ((nNbElts == (int)usDepth - 1) || (nNbElts == -1)) {
		// The FIFO is full
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "YES");
#endif
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] FIFO full for current reader %d",
			(m_sName == NULL ? "unknown" : m_sName), m_nReaderInd);
#endif
		return true;
	}
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "NO");
#endif
	return false;
}

bool isl::CSHMData::IsFifoFull()
{
	unsigned short usWrite = *m_usIndWrite;
	unsigned short usDepth = *m_usFifoDepth;
	int nReaders = *m_nReaders;
	if (nReaders == 0) {
		// No reader yet connected, we still need to wait then
		// use the first one which should be set to 0
		nReaders = 1;
	}
	for (int i = 0; i < nReaders; i++) {
		// If only one reader verify the condition
		// then the FIFO is considered as full
		unsigned short usRead = m_usIndReads[i];
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "FIFO full for reader[%d]? W:%d - R:%d ? %d",
			i, usWrite, usRead, usDepth);
#endif
		int nNbElts = (int)usWrite - (int)usRead;
		if ((nNbElts == (int)usDepth - 1) || (nNbElts == -1)) {
			// The FIFO is full
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG, "YES");
#endif
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] FIFO full for reader %d",
				(m_sName == NULL ? "unknown" : m_sName), i);
#endif
			return true;
		}
	}
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "NO");
#endif
	return false;
}

bool isl::CSHMData::IsFifoEmpty()
{
	if (m_nReaderInd == -1) {
		return false;
	}
	unsigned short usWrite = *m_usIndWrite;
	unsigned short usRead = m_usIndReads[m_nReaderInd];
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "FIFO empty? W:%d ? R:%d", usWrite, usRead);
#endif
	if (usWrite == usRead) {
		// The FIFO is empty
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "YES");
#endif
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] FIFO empty",
			(m_sName == NULL ? "unknown" : m_sName));
#endif
		return true;
	}
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "NO");
#endif
	return false;
}

bool isl::CSHMData::SetData(void * pData, double dTime, bool * bListen)
{
	if (IsReader()) {
		return false;
	}
	if (IsFifoFull()) {
		// Shall wait to write the value
		if (bListen != NULL) {
			int nVal = *m_nWriterListen;
			*m_nWriterListen = nVal + 1;
			*bListen = true;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG,
			"[S1] Set data for time: %gs. Wait for available space", dTime);
#endif
		return false;
	}
	unsigned short usInd = *m_usIndWrite;
	unsigned short usDepth = *m_usFifoDepth;
	int nInd = (int )usInd;
	int nSize = (*m_nSizeType)*(*m_nSize);
	//
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG,
		"[S2] Set data in pos:%d for time: %gs.", nInd, dTime);
#endif
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] Set Data(data, time:%g, listen:%d)",
		(m_sName == NULL ? "unknown" : m_sName), dTime, (bListen == NULL ? -1 : *bListen));
#endif
	void * pElement = (char *)m_pData + nInd * nSize;
	//memcpy(pElement, pData, nSize);
	MemCopy(pElement, pData, nSize, true);
	m_dTimes[nInd] = dTime;
	m_dSteps[nInd] = m_dOriginalStep;
	//
	if (usInd == usDepth - 1) {
		*m_usIndWrite = 0;
	}
	else {
		*m_usIndWrite = usInd + 1;
	}
	//
	if (bListen != NULL) {
		*bListen = false;
	}
	return true;
}

bool isl::CSHMData::SetData(void * pData, double dTime, double dStep, bool * bListen)
{
	if (IsReader()) {
		return false;
	}
	if (IsFifoFull()) {
		// Shall wait to write the value
		if (bListen != NULL) {
			int nVal = *m_nWriterListen;
			*m_nWriterListen = nVal + 1;
			*bListen = true;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG,
			"[ST1] Set data for time: %gs. Wait for available space", dTime);
#endif
		return false;
	}
	unsigned short usInd = *m_usIndWrite;
	unsigned short usDepth = *m_usFifoDepth;
	int nInd = (int)usInd;
	int nSize = (*m_nSizeType)*(*m_nSize);
	//
#ifdef ISL_DEBUG
	AppLogDebug(2 ,ISLSHMDATA_DEBUG, "[ST2] Set data in pos:%d for time: %gs.", nInd, dTime);
#endif
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] Set Data(data, time:%g, step:%g, listen:%d)",
		(m_sName == NULL ? "unknown" : m_sName), dTime, dStep, (bListen == NULL ? -1 : *bListen));
#endif
	void * pElement = (char *)m_pData + nInd * nSize;
	//memcpy(pElement, pData, nSize);
	MemCopy(pElement, pData, nSize, true);
	m_dTimes[nInd] = dTime;
	m_dSteps[nInd] = dStep;
	//
	if (usInd == usDepth - 1) {
		*m_usIndWrite = 0;
	}
	else {
		*m_usIndWrite = usInd + 1;
	}
	//
	if (bListen != NULL) {
		*bListen = false;
	}
	return true;
}

bool isl::CSHMData::SetLastData(double dTime, double dStep, bool * bListen)
{
	if (IsReader()) {
		return false;
	}
	if (IsFifoFull()) {
		// Shall wait to write the value
		if (bListen != NULL) {
			int nVal = *m_nWriterListen;
			*m_nWriterListen = nVal + 1;
			*bListen = true;
		}
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG,
			"[SL1] Set last data for time: %gs. Wait for available space", dTime);
#endif
		return false;
	}
	unsigned short usInd = *m_usIndWrite;
	unsigned short usIndm1 = usInd - 1;
	unsigned short usDepth = *m_usFifoDepth;
	if (usInd == 0) {
		usIndm1 = usDepth - 1;
	}
	int nInd = (int)usInd;
	int nIndm1 = (int)usIndm1;
	int nSize = (*m_nSizeType)*(*m_nSize);
	//
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG,
		"[SL2] Set data in pos:%d for time: %gs.", nInd, dTime);
#endif
	void * pData = (char *)m_pData + nIndm1 * nSize;
	void * pElement = (char *)m_pData + nInd * nSize;
	memcpy(pElement, pData, nSize);
	m_dTimes[nInd] = dTime;
	m_dSteps[nInd] = dStep;
	//
	if (usInd == usDepth - 1) {
		*m_usIndWrite = 0;
	}
	else {
		*m_usIndWrite = usInd + 1;
	}
	//
	if (bListen != NULL) {
		*bListen = false;
	}
	return true;
}

bool isl::CSHMData::GetData(void * pData, double * dTime, double * dStep, bool * bListen)
{
	if (IsReader() == false) {
		return false;
	}
	if (IsFifoEmpty()) {
		// Shall wait to write the value
		if (bListen != NULL) {
			int nVal = *m_nReaderListen;
			*m_nReaderListen = nVal + 1;
			*bListen = true;
		}
		return false;
	}
	unsigned short usInd = m_usIndReads[m_nReaderInd];
	unsigned short usDepth = *m_usFifoDepth;
	int nInd = (int)usInd;
	int nSize = (*m_nSizeType)*(*m_nSize);
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] Get Data(data, outtime, outstep, listen:%d)",
		(m_sName == NULL ? "unknown" : m_sName), (bListen == NULL ? -1 : *bListen));
#endif
	//
	void * pElement = (char *)m_pData + nInd * nSize;
	//memcpy(pData, pElement, nSize);
	MemCopy(pData, pElement, nSize, false);
	*dTime = m_dTimes[nInd];
	//
	if (bListen != NULL) {
		*bListen = false;
	}
	if (usInd == usDepth - 1) {
		m_usIndReads[m_nReaderInd] = 0;
	}
	else {
		m_usIndReads[m_nReaderInd] = usInd + 1;
	}
	//
	return true;
}

// TODO : Create a function that will get the closest value even if already read
// in the condition that the FIFO already contain read data.
bool isl::CSHMData::GetData(void * pData, double * dOutTime, double * dOutStep, double dInTime, bool * bListen)
{
	if (IsReader() == false) {
		return false;
	}
	int nSize = (*m_nSizeType)*(*m_nSize);
	unsigned short usDepth = *m_usFifoDepth;
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[G0] InTime: %gs - FIFO Depth=%u.", dInTime, usDepth);
#endif
	//
	unsigned short usInd = m_usIndReads[m_nReaderInd];
	int nInd = (int)usInd;
	double dTime = m_dTimes[nInd];
	// In case of event data, we cannot compare times
	if (round(dTime) == EVENT_DEF_TIME_VAL) {
		return GetData(pData, dOutTime, dOutStep, bListen);
	}
	double dStep = m_dSteps[nInd];
	double dTolerance = m_dStepTolerance * (dStep < 0.0 ? 0.0 : dStep);
	double dNextTime = dTime + dStep;
	bool bIsFifoEmpty = IsFifoEmpty();
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[G15] Step=%gs Time=%gs (InTime is %s than Time) - IsFifoEmpty? %d",
		dStep, dTime, (dInTime < dTime - dTolerance ? "Less" : "Greater"), bIsFifoEmpty);
#endif
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[%s] Get Data(data, outtime, outstep, intime:%g, listen:%d)",
		(m_sName == NULL ? "unknown" : m_sName), dInTime, (bListen == NULL ? -1 : *bListen));
#endif
	if (bIsFifoEmpty && (dInTime < dTime - dTolerance)) {
		int nPosOk = -1;
		double dTimeOk = 0.0;
		// We read from our current position down to 0
		unsigned short usCur = usInd;
		for (unsigned short i = 1; i < usDepth; i++) {
			usCur -= 1;
			if (usCur == MAX_FIFO_DEPTH) {
				usCur = usDepth - 1;
			}
			int j = (int)usCur;
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG,
				"[G11] Try to get on ind=%d (%d/%d), time=%gs.", j, usCur, usInd, m_dTimes[j]);
#endif
			if (m_dTimes[j] <= dInTime + dTolerance) {
				if ((m_dSteps[j] <= 0.0) && (m_dTimes[j] > dTimeOk)) {
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG, "[G12] We have one at %gs.", m_dTimes[j]);
#endif
					dTimeOk = m_dTimes[j];
					nPosOk = j;
				}
				else if (m_dTimes[j] + m_dSteps[j] > dInTime + dTolerance) {
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG, "[G20] We have one at %gs.", m_dTimes[j]);
#endif
					nPosOk = j;
				}
			}
		}
		if (bListen != NULL) {
			*bListen = false;
		}
		if (nPosOk == -1) {
			// No value found
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG, "[G13] No value found for t=%gs.", dInTime);
#endif
			return false;
		}
		void * pElement = (char *)m_pData + nPosOk * nSize;
		//memcpy(pData, pElement, nSize
		MemCopy(pData, pElement, nSize, false);
		*dOutStep = m_dSteps[nPosOk];
		*dOutTime = m_dTimes[nPosOk];
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "[G14] Get data in pos:%d for time: %gs.",
			nPosOk, *dOutTime);
#endif
		return true;
	}
	else {
		//
		while (bIsFifoEmpty == false) {
			usInd = m_usIndReads[m_nReaderInd];
			nInd = (int)usInd;
			dTime = m_dTimes[nInd];
			dStep = m_dSteps[nInd];
			dTolerance = m_dStepTolerance * (dStep < 0.0 ? 0.0 : dStep);
			dNextTime = dTime + dStep;
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG,
				"[G10] Next time: time:%gs + step:%gs = nexttime:%gs.",	dTime, dStep, dNextTime);
#endif
			if (dInTime > dTime + dTolerance) {
				if ((dStep >= 0.0) && (dInTime >= dNextTime - dTolerance)) {
					// Try to get the next value
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG,
						"[G1] Get data in pos:%d for time: %gs. Try to get a new one", nInd, dTime);
#endif
					if (usInd == usDepth - 1) {
						m_usIndReads[m_nReaderInd] = 0;
					}
					else {
						m_usIndReads[m_nReaderInd] = usInd + 1;
					}
					// We continue to read
				}
				else {
					// We have the right value
					*dOutStep = dStep;
					*dOutTime = dTime;
					void * pElement = (char *)m_pData + nInd * nSize;
					//memcpy(pData, pElement, nSize);
					MemCopy(pData, pElement, nSize, false);
					if (bListen != NULL) {
						*bListen = false; // We found it
					}
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG,
						"[G2] Get data in pos:%d for time: %gs.", nInd, dTime);
#endif
					if (usInd == usDepth - 1) {
						m_usIndReads[m_nReaderInd] = 0;
					}
					else {
						m_usIndReads[m_nReaderInd] = usInd + 1;
					}
					return true;
				}
			}
			else if ((dInTime >= dTime - dTolerance) && (dInTime <= dTime + dTolerance)) {
				// We have the right value - To avoid a search in the FIFO
				*dOutStep = dStep;
				*dOutTime = dTime;
				void * pElement = (char *)m_pData + nInd * nSize;
				//memcpy(pData, pElement, nSize);
				MemCopy(pData, pElement, nSize, false);
				if (bListen != NULL) {
					*bListen = false; // We found it
				}
#ifdef ISL_DEBUG
				AppLogDebug(2, ISLSHMDATA_DEBUG,
					"[G3] Get data in pos:%d for time: %gs.", nInd, dTime);
#endif
				if (usInd == usDepth - 1) {
					m_usIndReads[m_nReaderInd] = 0;
				}
				else {
					m_usIndReads[m_nReaderInd] = usInd + 1;
				}
				return true;
			}
			else { // dInTime < dTime - dTolerance
				int nPosOk = -1;
				double dTimeOk = 0.0;
				// We read from our current position down to 0
				unsigned short usCur = usInd;
				for (unsigned short i = 1; i < usDepth; i++) {
					usCur -= 1;
					if (usCur == MAX_FIFO_DEPTH) {
						usCur = usDepth - 1;
					}
					int j = (int)usCur;
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG,
						"[G7] Try to get on ind=%d (%d/%d), time=%gs[ts=%gs].",
						j, usCur, usInd, m_dTimes[j], m_dSteps[j]);
#endif
					if (m_dTimes[j] <= dInTime + dTolerance) {
						if ((m_dSteps[j] <= 0.0) && (m_dTimes[j] > dTimeOk)) {
#ifdef ISL_DEBUG
							AppLogDebug(2, ISLSHMDATA_DEBUG, "[G9] We have one at %gs.", m_dTimes[j]);
#endif
							dTimeOk = m_dTimes[j];
							nPosOk = j;
						}
						else if (m_dTimes[j] + m_dSteps[j] > dInTime + dTolerance) {
#ifdef ISL_DEBUG
							AppLogDebug(2, ISLSHMDATA_DEBUG,
								"[G21] We have one at %gs.", m_dTimes[j]);
#endif
							nPosOk = j;
						}
					}
				}
				if (bListen != NULL) {
					*bListen = false;
				}
				if (nPosOk == -1) {
					// No value found
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG, "[G8] No value found for t=%gs.", dInTime);
#endif
					return false;
				}
				void * pElement = (char *)m_pData + nPosOk * nSize;
				//memcpy(pData, pElement, nSize);
				MemCopy(pData, pElement, nSize, false);
				*dOutStep = m_dSteps[nPosOk];
				*dOutTime = m_dTimes[nPosOk];
				// We increment the FIFO reader index only if the current value is the good one.
				if (nPosOk == nInd) {
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG, "[G4] Get data in pos:%d for time: %gs.",
						nPosOk, *dOutTime);
#endif
					// We read the last value
					if (usInd == usDepth - 1) {
						m_usIndReads[m_nReaderInd] = 0;
					}
					else {
						m_usIndReads[m_nReaderInd] = usInd + 1;
					}
				}
#ifdef ISL_DEBUG
				else {
					AppLogDebug(2, ISLSHMDATA_DEBUG, "[G5] Get data in pos:%d for time: %gs.",
						nPosOk, *dOutTime);
				}
#endif
				return true;
			}
			bIsFifoEmpty = IsFifoEmpty();
		}
	}
	// If we are above current time and Fifo is empty
	if ((dInTime >= dTime - dTolerance) && bIsFifoEmpty) {
		// We read the most recent value
		int nPosOk = -1;
		double dTimeOk = -1.0;
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "[G22] (dInTime >= dTime - dTolerance) && bIsFifoEmpty.");
#endif
		// We read from our current position down to 0
		unsigned short usCur = usInd;
		for (unsigned short i = 1; i < usDepth; i++) {
			usCur -= 1;
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG, "[G23] For i=%d to usDepth=%d, usCur=%d, MAX_FIFO_DEPTH=%d.",
				i, usDepth, usCur, MAX_FIFO_DEPTH);
#endif
			if (usCur == MAX_FIFO_DEPTH) {
				usCur = usDepth - 1;
#ifdef ISL_DEBUG
				AppLogDebug(2, ISLSHMDATA_DEBUG, "[G24] usCur=%d.", usCur);
#endif
			}
			int j = (int)usCur;
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG, "[G25] j=%d.", j);
#endif
			if (m_dSteps[j] < 0.0) {
#ifdef ISL_DEBUG
				AppLogDebug(2, ISLSHMDATA_DEBUG, "[G16] Try to get on ind=%d (%d/%d), time=%gs.",
					j, usCur, usInd, m_dTimes[j]);
#endif
				if ((dInTime >= m_dTimes[j] - dTolerance) && (m_dTimes[j] > dTimeOk)) {
#ifdef ISL_DEBUG
					AppLogDebug(2, ISLSHMDATA_DEBUG, "[G17] We have one at time %gs.", m_dTimes[j]);
#endif
					dTimeOk = m_dTimes[j];
					nPosOk = j;
				}
			}
		}
		if (nPosOk != -1) {
			if (bListen != NULL) {
				*bListen = false; // We found it
			}
			// One value found
			void * pElement = (char *)m_pData + nPosOk * nSize;
			//memcpy(pData, pElement, nSize);
			MemCopy(pData, pElement, nSize, false);
			*dOutStep = m_dSteps[nPosOk];
			*dOutTime = m_dTimes[nPosOk];
#ifdef ISL_DEBUG
			AppLogDebug(2, ISLSHMDATA_DEBUG, "[G19] Get data in pos:%d for time: %gs.",
				nPosOk, *dOutTime);
#endif
			return true;
		}
	}
	// Wait for the next value
	if (bListen != NULL) {
		int nVal = *m_nReaderListen;
		*m_nReaderListen = nVal + 1;
		*bListen = true;
	}
#ifdef ISL_DEBUG
	AppLogDebug(2, ISLSHMDATA_DEBUG, "[G6] Get data for time: %gs. Wait for a new value.", dInTime);
#endif
	return false;
}

bool isl::CSHMData::SetData(void * pData, double dTime, int nInd)
{
	unsigned short usDepth = *m_usFifoDepth;
	if (nInd >= (int)usDepth) {
		return false;
	}
	unsigned short usInd = *m_usIndWrite;
	// Start just after the ucIndWrite
	for (int i = 0; i <= nInd; i++) {
		if (usInd == usDepth - 1) {
			usInd = 0;
		}
		else {
			usInd += 1;
		}
	}
	nInd = (int)usInd;
	int nSize = (*m_nSizeType)*(*m_nSize);
	void * pElement = (char *)m_pData + nInd*nSize;
	memcpy(pElement, pData, nSize);
	m_dTimes[nInd] = dTime;
	return true;
}

bool isl::CSHMData::GetData(void * pData, double * dTime, int nInd)
{
	unsigned short usDepth = *m_usFifoDepth;
	int nDepth = (int)usDepth;
	if (nInd >= nDepth) {
		return false;
	}
	if (nInd <= -nDepth) {
		return false;
	}
	unsigned short usInd = m_usIndReads[m_nReaderInd];
	if (nInd < 0) {
		for (int i = 0; i >= nInd; i--) {
			if (usInd == 0) {
				usInd = usDepth - 1;
			}
			else {
				usInd -= 1;
			}
		}
	}
	else {
		// Start just after the m_ucIndReads[m_nReaderInd]
		for (int i = 0; i <= nInd; i++) {
			if (usInd == usDepth - 1) {
				usInd = 0;
			}
			else {
				usInd += 1;
			}
		}
	}
	nInd = (int)usInd;
	int nSize = (*m_nSizeType)*(*m_nSize);
	void * pElement = (char *)m_pData + nInd*nSize;
	memcpy(pData, pElement, nSize);
	*dTime = m_dTimes[nInd];
	return true;
}

bool isl::CSHMData::GetMemData(void * pData, double * dTime, double * dStep, int nInd)
{
	unsigned short usDepth = *m_usFifoDepth;
	if (nInd >= (int)usDepth) {
		return false;
	}
	if (nInd < 0) {
		return false;
	}
	int nSize = (*m_nSizeType)*(*m_nSize);
	void * pElement = (char *)m_pData + nInd*nSize;
	memcpy(pData, pElement, nSize);
	*dTime = m_dTimes[nInd];
	*dStep = m_dSteps[nInd];
	return true;
}

bool isl::CSHMData::MemCopy(void * pDst, void * pSrc, size_t nSize, bool bToSHM)
{
	// No check on pointers, we suppose a correct usage
	if (*m_eType != CDataType::TP_STRUCTURE) {
		memcpy(pDst, pSrc, nSize);
	}
	else {
#ifdef ISL_DEBUG
		AppLogDebug(2, ISLSHMDATA_DEBUG, "MemCpy on structure (dimension in bytes: %d)", nSize);
#endif
		// TODO: Arrays of complex data are not managed yet
		int nNbFields = m_cType->GetNbOfSubTypes();
		if (nNbFields <= 0) {
			AppLogError(ISLSHMDATA_WRONG_STRUCTURE, "Variable %s: wrong structured type.",
				m_cParent->GetId().c_str());
			return false;
		}
		if (bToSHM) {
			int i = 0;
			char * pNextDst = (char *)pDst;
			void ** pNextSrc = (void **)pSrc;
			for (int i = 0; i < nNbFields; i++) {
				CDataType * cFieldType = m_cType->GetSubType(i);
				int nLocalSize = cFieldType->GetSizeInBytes();
				memcpy(pNextDst, pNextSrc[i], nLocalSize);
#ifdef ISL_DEBUG
				AppLogDebug(2, ISLSHMDATA_DEBUG, "SET - Item of size %d from %g to %g",
					nLocalSize, *((double *)pNextDst), *((double *)(pNextSrc[i])));
#endif
				pNextDst = pNextDst + nLocalSize;
				i++;
			}
		}
		else {
			int i = 0;
			char * pNextSrc = (char * )pSrc;
			void ** pNextDst = (void **)pDst;
			for (int i = 0; i < nNbFields; i++) {
				CDataType * cFieldType = m_cType->GetSubType(i);
				int nLocalSize = cFieldType->GetSizeInBytes();
				memcpy(pNextDst[i], pNextSrc, nLocalSize);
#ifdef ISL_DEBUG
				AppLogDebug(2, ISLSHMDATA_DEBUG, "GET - Item of size %d from %g to %g",
					nLocalSize, *((double *)pNextSrc), *((double *)(pNextDst[i])));
#endif
				pNextSrc = pNextSrc + nLocalSize;
				i++;
			}
		}
	}
	return true;
}