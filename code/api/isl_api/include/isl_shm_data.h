/*
 *     Name: isl_shm_data.h
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

#ifndef _ISL_SHM_DATA_H_
#define _ISL_SHM_DATA_H_

/*
 *     Header files
 */

#include <string>


/*
 *     Classes declaration
 */

namespace isl {
	class CDataType;
	class CData;

	class CSHMData
	{
	public:
		static int GetSizeOf(CData * cData);

		CSHMData(void * pData, CData * cData);
		~CSHMData();

		bool Initialize();
		bool InitializeData(double dTime);

		unsigned int GetId();
		std::string GetName();

		CDataType::tType GetType();
		int GetSizeType();
		int GetSize();

		unsigned short GetFifoDepth();
		unsigned short GetIndWriter();
		unsigned short GetIndReader(int i);

		void SetReader();
		bool IsReader();
		int GetReaders();

		int GetReaderListen();
		void SetReaderListen(int nVal);

		int GetWriterListen();
		void SetWriterListen(int nVal);

		void SetTerminated();
		bool IsTerminated();

		bool IsFifoFullForReader();
		bool IsFifoFull();
		bool IsFifoEmpty();

		bool SetData(void * pData, double dTime, bool * bListen);
		bool SetData(void * pData, double dTime, double dStep, bool * bListen);
		bool SetLastData(double dTime, double dStep, bool * bListen);

		bool GetData(void * pData, double * dTime, double * dStep, bool * bListen);
		bool GetData(void * pData, double * dOutTime, double * dOutStep, double dInTime, bool * bListen);

		bool SetData(void * pData, double dTime, int nInd);
		bool GetData(void * pData, double * dTime, int nInd);

		bool GetMemData(void * pData, double * dTime, double * dStep, int nInd);

	private:
		bool MemCopy(void * pDst, void * pSrc, size_t nSize, bool bToSHM);

	private:
		unsigned int * m_uId;

		int * m_nSizeName;
		char * m_sName; // Actually the Id of the variable

		CDataType * m_cType;
		CDataType::tType * m_eType;
		int * m_nSizeType;
		int * m_nSize;
		//std::vector<size_t> m_lSizes;

		unsigned short * m_usIndWrite;
		int * m_nReaders;
		unsigned short * m_usIndReads;

		int * m_nReaderListen;
		int * m_nWriterListen;

		bool * m_bIsTerminated;

		unsigned short * m_usFifoDepth;
		double * m_dTimes;
		double * m_dSteps;
		void * m_pData;

		CData * m_cParent;
		int m_nReaderInd;

		double m_dOriginalStep;
		double m_dStepTolerance;
	};
}

#endif // _ISL_SHM_DATA_H_
