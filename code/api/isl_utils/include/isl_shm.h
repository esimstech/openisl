/*
 *     Name: isl_shm.h
 *
 *     Description: Shared Memory tools.
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

#ifndef _ISL_SHM_H_
#define _ISL_SHM_H_

/*
 *     Header files
 */

#include <string>
#ifndef WIN32
#include <sys/types.h>
#endif
#include <isl_sem.h>


/*
 *     Types definition
 */

#ifdef WIN32
typedef void * tISLSHMHandle;
#else
typedef struct structISLSHMHandle {
	key_t nKey;
	bool bFileCreated;
} tsISLSHMHandle;
typedef tsISLSHMHandle * tISLSHMHandle;
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
			REMOVEERROR,
			FILEKEYNOTREMOVED,
			UNKNOWN
		};

		static const std::string c_sPrefix;
		static const size_t c_nPrefixMinSize;

		CSHM(const std::string & sKey, bool bIsGlobal = false);
		CSHM(const std::string & sKey, const std::string & sPrefix, bool bIsGlobal = false);
		~CSHM();

		tStatus GetStatus();
		int GetError();
		CSem::tStatus GetSemStatus();

		void SetPrefix(const std::string & sPrefix);
		void SetKey(const std::string & sKey, bool bIsGlobal = false);
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

		bool m_bIsGlobal;
		tStatus m_eStatus;
		int m_nError;

		CSem m_cSem;
		bool m_bLockedByMe;
		bool m_bCreationMode;
	};
}

#endif // _IPI_SHM_H_
