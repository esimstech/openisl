/*
 *     Name: isl_shm_data.h
 *
 *     Description: ISL API SHM Data class.
 *
 *     Modification date: 09/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
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
