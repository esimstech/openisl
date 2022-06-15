/*
 *     Name: isl_shm.h
 *
 *     Description: Shared Memory tools.
 *
 *     Modification date: 24/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_SHM_H_
#define _ISL_SHM_H_

/*
 *     Header files
 */

#include <string>

#include <isl_sem.h>


/*
 *     Types definition
 */

#ifdef WIN32
typedef void * tISLSHMHandle;
#else
typedef int tISLSHMHandle;
#endif


/*
 *     Classes declaration
 */

namespace isl {
	class CSHM
	{
	public:
		enum tAccessMode {
			READONLY = 0,
			READWRITE
		};

		enum tStatus {
			SUCCESS = 0,
			NOKEY,
			WRONGKEYFORMAT,
			NOINTKEY,
			PERMISSIONDENIED,
			INVALIDSIZE,
			KEYERROR,
			ALREADYEXISTS,
			NOTFOUND,
			LOCKERROR,
			UNLOCKERROR,
			LOCKCREATEERROR,
			OUTOFRESOURCES,
			OUTOFMEMORY,
			SEMAPHOREERROR,
			LOCKATTACHERROR,
			ATTACHERROR,
			GETSIZEERROR,
			LOCKDETACHERROR,
			DETACHERROR,
			UNKNOWN
		};

		static const std::string c_sPrefix;
		static const size_t c_nPrefixMinSize;

		CSHM(const std::string & sKey);
		CSHM(const std::string & sKey, const std::string & sPrefix);
		~CSHM();

		tStatus GetStatus();
		int GetError();
		CSem::tStatus GetSemStatus();

		void SetPrefix(const std::string & sPrefix);
		void SetKey(const std::string & sKey);
		std::string GetKey();
		std::string GetName();

		bool Create(int nSize, tAccessMode eMode = READWRITE);
		int Size();

		bool Attach(tAccessMode eMode = READWRITE);
		bool IsAttached();
		bool Detach();

		void * Data();

		bool Lock();
		bool Unlock();

	protected:
		std::string BuildName();

		bool Init();
		bool Get();
		bool Create();
		bool Close();

	private:
		tISLSHMHandle m_cSHM;

		void * m_pData;
		int m_nSize;

		std::string m_sPrefix;
		std::string m_sKey;
		std::string m_sName;

		tStatus m_eStatus;
		int m_nError;

		CSem m_cSem;
		bool m_bLockedByMe;
		bool m_bCreationMode;
	};
}

#endif // _IPI_SHM_H_
