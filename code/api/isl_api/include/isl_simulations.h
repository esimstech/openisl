/*
 *     Name: isl_simulations.h
 *
 *     Description: ISL simulation instances management.
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
