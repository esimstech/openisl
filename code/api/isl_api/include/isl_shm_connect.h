/*
 *     Name: isl_shm_connect.h
 *
 *     Description: ISL API SHM Connect class.
 *
 *     Modification date: 14/04/2019
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_SHM_CONNECT_H_
#define _ISL_SHM_CONNECT_H_

/*
 *     Header files
 */

#include <string>


/*
 *     Constants definition
 */

#define SHM_MODEL_KEY_ID		"_isl_shm_ses%1%_mdl%2%"


/*
 *     Classes declaration
 */

/*
 *     Class CSHMConnect
 */

namespace isl {
	class CConnect;

	class CSHMConnect
	{
	public:
		static int GetSizeOf(CConnect * cConnect);

		CSHMConnect(void * pData, CConnect * cConnect);
		~CSHMConnect();

		bool Initialize();

		unsigned int GetType();
		unsigned int GetId();
		unsigned long GetPID();

		std::string GetUUID();
		std::string GetName();
		std::string GetFile();

		int GetNbData();

	private:
		unsigned int * m_uType; // Compatibility with the enum type of ISL

		unsigned int * m_uId;
		unsigned long * m_ulPID;

		int * m_nSizeUUID;
		char * m_sUUID;

		int * m_nSizeName;
		char * m_sName;

		int * m_nSizeFile;
		char * m_sFile;

		int * m_nNbData;

		CConnect * m_cParent;
	};
}

#endif // _ISL_SHM_CONNECT_H_
