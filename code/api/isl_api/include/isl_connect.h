/*
 *     Name: isl_connect.h
 *
 *     Description: ISL connection element representing an instance of a couple simulator/model.
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

#ifndef _ISL_CONNECT_H_
#define _ISL_CONNECT_H_

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
	class CDataType;
	class CVariable;
	class CData;

	class ISL_API_EXPORT CConnect
	{
	public:

		enum tStopMode {
			SMD_STOP = 0,
			SMD_EXIT,
			SMD_UNKNOWN
		};

		// General methods to get static information
		static std::string GetVersionNumber();
		static std::string GenUID();

		// ISL co-simulation stop control methods
		static bool StartThreadListenToStop(CConnect * cConnect);
		static bool SendStopRequest();
		static bool SendStopSession(const std::string & sSession);

		CConnect(); // means bOwner = true
		CConnect(bool bOwner);
		~CConnect();

		bool New(const std::string & sName);
		bool Load(const std::string & sFileName);
		bool Check();
		bool Save(const std::string & sFileName = "");

		void SetName(const std::string & sName);
		std::string GetName();
		std::string GetId(); // Id built by CConnect or loaded from the original file
		unsigned int GetUId();
		void SetType(const std::string & sType); // Only the first 4 characters are used
		unsigned int GetType();
		std::string GetTypeAsStr();
		void SetPID(unsigned long ulPId);
		unsigned long GetPID();
		std::string GetFileName();

		bool SetSessionId(const std::string & sSessionId);
		std::string GetSessionId();
		void SetConnectTimeOut(int nTimeOut);
		int GetConnectTimeOut();
		void SetStartTime(double dVal);
		double GetStartTime();
		void SetEndTime(double dVal);
		double GetEndTime();
		void SetStepSize(double dVal);
		double GetStepSize();
		void SetStepTolerance(double dVal);
		double GetStepTolerance();

		CData * NewIO(const std::string & sId, CVariable::tCausality eCausality, CDataType::tType eType, int nSize = 1);
		bool AddIO(CData * cData);
		bool RemoveIO(CData * cData);

		int GetNbIOs();
		int GetNbInputs();
		int GetNbOutputs();

		bool IOExists(const std::string & sId);
		CData * GetIO(const std::string & sId, int * nInd = 0);
		CData * GetIO(int i);
		CData * GetInput(const std::string & sId);
		CData * GetInput(int i);
		CData * GetOutput(const std::string & sId);
		CData * GetOutput(int i);

		void CloseLogOnDelete(bool bVal);
		void SetMode(tStopMode eMode);
		tStopMode GetMode();
		void SetViewer(bool bVal);
		bool IsViewer();

		void SetTerminated(bool bVal = true);
		bool IsTerminated();
		bool IsConnnected();

		bool Create(const std::string & sSessionId = "");
		bool Connect(bool bWait = true);
		bool Disconnect();

	private:
		bool ConnectAsViewer(bool bWait);
		bool DisconnectAsViewer();

	public:
		void StartTimer();
		void StopTimer();

		bool ListenToExitSession();
		bool SendStopSession();

#if 0 // TODO: To implement
		bool OpenStore();
		void * GetStore();
		bool CloseStore();
#endif

	private:
		std::string m_sName;
		std::string m_sId;
		unsigned int m_uType;
		unsigned long m_ulPID;

		std::string m_sFileName;
		std::string m_sSessionId;
		int m_nConnectTimeOut; // in seconds
		double m_dStartTime;
		double m_dEndTime;
		double m_dStepSize;
		double m_dStepTolerance;

		std::vector<CData *> m_lIOs;
		std::vector<CData *> m_lIns;
		std::vector<CData *> m_lOuts;

		std::map<std::string, CData *> m_mIOs;
		std::map<std::string, CData *> m_mIns;
		std::map<std::string, CData *> m_mOuts;

		CSHM * m_cContainer;
		CSHMConnect * m_cData;

		bool m_bCloseLog;
		bool m_bManager;
		tStopMode m_eMode;
		bool m_bViewer;
		bool m_bTerminated;

		unsigned char m_ucState; // 1: Completed, 3: Session created, 7: Transmitters connected

#if 0 // TODO: To add
		void * m_cStore;
#endif
		void * m_cSimData;

		time_t m_lTimer;
	};
}

#endif // _ISL_CONNECT_H_
