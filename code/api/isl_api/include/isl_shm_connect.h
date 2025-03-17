/*
 *     Name: isl_shm_connect.h
 *
 *     Description: ISL API SHM Connect class.
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
