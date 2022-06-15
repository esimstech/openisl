/*
 *     Name: isl_sem.h
 *
 *     Description: Semaphore tools.
 *
 *     Modification date: 20/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_SEM_H_
#define _ISL_SEM_H_

 /*
  *     Header files
  */

#include <string>
#ifndef WIN32
#include <semaphore.h>
#endif

/*
 *     Types definition
 */

#ifdef WIN32
typedef void * tISLSemHandle;
#else
typedef sem_t * tISLSemHandle;
#endif


/*
 *     Classes declaration
 */

namespace isl {
	class CSem
	{
	public:
		enum tAccessMode	{
			OPEN = 0,
			CREATE
		};

		enum tStatus {
			SUCCESS = 0,
			NOKEY,
			WRONGKEYFORMAT,
			PERMISSIONDENIED,
			KEYERROR,
			ALREADYEXIST,
			NOTFOUND,
			OUTOFRESOURCES,
			OUTOFMEMORY,
			RELEASEFAILED,
			ACQUIREFAILED,
			TIMEOUTREACHED,
			UNKNOWN
		};

		static const std::string c_sPrefix;
		static const size_t c_nPrefixMinSize;

		CSem(const std::string & sKey, int nInitVal = 0, tAccessMode eMode = OPEN);
		CSem(const std::string & sKey, const std::string & sPrefix, int nInitVal = 0, tAccessMode eMode = OPEN);
		~CSem();

		void SetTimeout(int nTimeout); // milliseconds

		void SetPrefix(const std::string & sPrefix);
		void SetKey(const std::string & sKey, int nInitVal = 0, tAccessMode eMode = OPEN);
		std::string GetKey();
		std::string GetName();

		bool Acquire();
		bool Release(int n = 1);

		tStatus GetStatus();
		int GetError();

	protected:
		std::string BuildName();

		bool Create();
		bool Close();
		bool Modify(int n);

	private:
		tISLSemHandle m_cSem;

		std::string m_sPrefix;
		std::string m_sKey;
		std::string m_sName;
		
		int m_nInitVal;
		int m_nTimeout;

		tAccessMode m_eMode;
		tStatus m_eStatus;
		int m_nError;
	};
}

#endif // _IPI_SEM_H_
