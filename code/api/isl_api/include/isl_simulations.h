/*
 *     Name: isl_simulations.h
 *
 *     Description: ISL simulation instances management.
 *
 *     Modification date: 16/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_SIMULATIONS_H_
#define _ISL_SIMULATIONS_H_

/*
 *     Header files
 */

#include <time.h>
#include <string>
#include <vector>
#include <map>


/*
 *     Classes declaration
 */

namespace isl {
	class CSHMConnect;
	class CSHM;

	class ISL_API_EXPORT CSimulations
	{
	public:
		static const std::string c_sKeyGeneralId;

		CSimulations();
		~CSimulations();

		unsigned int GetMaxStringSize();
		int GetMaxNb();

		int GetSizeOneSim();

		unsigned int GetType();

		void SetId(unsigned int uId);
		unsigned int GetId();

		void SetPID(unsigned long ulPID);
		unsigned long GetPID();

		int GetNbIOs();

		std::string GetUUID();

		void SetName(const std::string & sName);
		std::string GetName();

		std::string GetFile();

		void SetSessionId(const std::string & sSessionId);
		std::string GetSessionId();

		void * Add();
		bool Remove(void * pASimData);
		bool Get(int nInd);

	private:
		bool Connect();
		bool Disconnect();
		bool Initialize();

	private:
		static CSimulations * m_cInstance;

		unsigned int m_uMaxStrSize;
		int m_nMaxNbSims;

		int m_nSizeId;			// ISL id of a model
		int m_nSizePID;			// PID of a model
		int m_nSizeSizeName;
		int m_nSizeName;		// Name of a model
		int m_nSizeSizeSession;
		int m_nSizeSession;		// Name of a session

		int m_nSize;

		unsigned int m_uAType;
		unsigned int m_uAId;
		unsigned long m_ulAPID;
		int m_nANbIOs;
		std::string m_sAUUID;
		std::string m_sAName;
		std::string m_sAFile;
		std::string m_sASessionId;

		CSHM * m_cContainer;
		CSHMConnect * m_cData;

	public:
		static CSimulations * Instance();
		static void Close();

		static int GetMaxNbSimulations();
	};
}

#define ISLSims isl::CSimulations::Instance()
#define ISLSims_Close isl::CSimulations::Close()

#endif // _ISL_SIMULATIONS_H_
