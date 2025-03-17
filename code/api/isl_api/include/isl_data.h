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

#ifndef _ISL_DATA_H_
#define _ISL_DATA_H_

/*
 *     Header files
 */

#include <string>
#include <vector>

/*
 *     Classes declaration
 */

namespace isl {
	class CSHMData;
	class CSem;
	class CSHM;

	class ISL_API_EXPORT CData : public CVariable
	{
	public:
		CData(CConnect * cParent, const std::string & sId);
		~CData();

		void SetFifoDepth(unsigned short usDepth);
		unsigned short GetFifoDepth();
		unsigned short GetMaxFifoDepth();

		bool IsFifoFull();
		bool IsFifoEmpty();

		int GetReaders();
		unsigned char GetIndWriter();
		unsigned char GetIndReader(int i);

		int GetMaxReaders();

		bool Initialize(double dTime);

		bool SetData(void * pData, double dTime, bool bWait);
		bool SetData(void * pData, double dTime, double dStep, bool bWait);
		bool SetLastData(double dTime, double dStep, bool bWait);

		bool GetData(void * pData, double * dTime, bool bWait);
		bool GetData(void * pData, double * dOutTime, double dInTime, bool bWait);
		bool GetDataAndStep(void * pData, double * dTime, double * dStep, bool bWait);

		bool SetEventData(void * pData, bool bWait);
		bool GetEventData(void * pData, bool bWait);

		bool SetData(void * pData, double dTime, int nInd);
		bool GetData(void * pData, double * dTime, int nInd);

		bool GetMemData(void * pData, double * dTime, double * dStep, int nInd);

		bool StoreData(void * pData, double dTime);

		int Connect(bool bWait, int nTimeOut = -1);
		bool Disconnect();

		bool ConnectAsViewer(bool bWait);
		bool DisconnectAsViewer();

		bool IsConnected();

		int GetComputeType();
		void SetCompute(void * cCompute);
		void * GetCompute();

		double GetOriginalStep();
		double GetStepTolerance();

	protected:
		void InitOnCheck();

	private:
		CSem * m_cWriterListen;
		CSem * m_cReaderListen;

		CSHM * m_cContainer;
		CSHMData * m_cData;

		unsigned short m_usMaxFifoDepth;
		unsigned short m_usFifoDepth;

		int m_nMaxNbReaders;

		bool m_bManager;
		bool m_bIsConnected;
		bool m_bIsViewer;

		double m_dOriginalStep;
		double m_dStepTolerance;

		double m_dTmpStep; // Used by GetDataAndStep

		// TODO: Implement void * m_cCompute;
	};

}

#endif // _ISL_DATA_H_
