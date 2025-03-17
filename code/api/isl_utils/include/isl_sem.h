/*
 *     Name: isl_sem.h
 *
 *     Description: Semaphore tools.
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

#ifndef _ISL_SEM_H_
#define _ISL_SEM_H_

 /*
  *     Header files
  */

#include <string>
#ifndef WIN32
#include <sys/types.h>
#endif

/*
 *     Types definition
 */

#ifdef WIN32
typedef void * tISLSemHandle;
#else
typedef struct structISLSemHandle {
	key_t nKey;
	int nSemaphore;
	bool bFileCreated;
	bool bSemCreated;
} tsISLSemHandle;
typedef tsISLSemHandle * tISLSemHandle;
#endif


/*
 *     Classes declaration
 */

namespace isl {
	class CIPCGlobalPrivate;
	class CIPCGlobal
	{
	private:
		// To start CIPCGlobal instance when the program starts...
		// ... and to clean CIPCGlobal when the program stops
		class CMngtIPCGlobal {
		private:
			CIPCGlobal * m_cInstance;

		public:
			CMngtIPCGlobal();
			~CMngtIPCGlobal();
			CIPCGlobal * Get();
		};

		CIPCGlobal();
		~CIPCGlobal();

	public:
		enum tStatus {
			SUCCESS = 0,
			OPEN_FAILED,
			LOOKUP_FAILED,
			ADJUST_FAILED,
			RESTORE_FAILED,
			UNKNOWN
		};

		// Avoid a copy
		CIPCGlobal(const CIPCGlobal&) = delete;
		CIPCGlobal& operator=(const CIPCGlobal&) = delete;

		bool UpdateSecurity(void * pSD, unsigned long ulAccessMask);

		const std::string GetPrefix();

		tStatus GetStatus();
		int GetError();

		static CIPCGlobal * Instance();

	private:
		CIPCGlobalPrivate * m_cPrivate;
		static CMngtIPCGlobal m_cMngtInstance;

		static const std::string c_sPrefix;

		tStatus m_eStatus;
		int m_nError;
	};

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
			HANDLEFAILED,
			CLOSEFAILED,
			MODIFYFAILED,
			UNKNOWN
		};

		static const std::string c_sPrefix;
		static const size_t c_nPrefixMinSize;

		CSem(const std::string & sKey, int nInitVal = 0,
			tAccessMode eMode = OPEN, bool bIsGlobal = false);
		CSem(const std::string & sKey, const std::string & sPrefix, int nInitVal = 0,
			tAccessMode eMode = OPEN, bool bIsGlobal = false);
		~CSem();

		void SetTimeout(int nTimeout); // milliseconds

		void SetPrefix(const std::string & sPrefix);
		void SetKey(const std::string & sKey, int nInitVal = 0,
			tAccessMode eMode = OPEN, bool bIsGlobal = false);
		std::string GetKey();
		std::string GetName();

		bool Acquire();
		bool Release(int n = 1);

		tStatus GetStatus();
		int GetError();

	protected:
		std::string BuildName();

		bool Create(tAccessMode eMode = OPEN);
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
		bool m_bIsGlobal;
		tStatus m_eStatus;
		int m_nError;
	};
}

#endif // _IPI_SEM_H_
