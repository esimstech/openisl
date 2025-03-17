/* 
 *     Name: isl_omnetpp.h
 *
 *     Description: OMNet++ coupling for OpenISL.
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

#ifndef _ISL_OMNETPP_H_
#define _ISL_OMNETPP_H_

/*
 *     Header files
 */

#include <string>
#include <map>
#include <isl_omnetpp_global.h>


/*
 *     Classes pre-declaration
 */

namespace isl {
	class CConnect;
	class CData;
}
class CISLOMNetIO;


/*
 *     Functions declaration
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	ISL_OMNETPP_EXPORT isl::CConnect * ISLInitialize(const char * sProcessName,
		const char * sSessionId, double dStartTime, double dEndTime, double dStepSize, int & nStatus);
	ISL_OMNETPP_EXPORT int ISLConnect(const char * sName, const char * sSessionId);
	ISL_OMNETPP_EXPORT int ISLTerminate(const char * sName, const char * sSessionId);

	ISL_OMNETPP_EXPORT isl::CData * ISLAddRealIO(const char * sName, const char * sSessionId, const char * sId,
		const char * sConnectId, double dInitVal, bool bIsInput, int & nStatus);

	ISL_OMNETPP_EXPORT int ISLGetReal(isl::CData * cData, double dTime, double & dVal, double & dRetTime);
	ISL_OMNETPP_EXPORT int ISLSetReal(isl::CData * cData, double dTime, double dVal);

	ISL_OMNETPP_EXPORT int ISLCheckAndSetReal(isl::CData * cData, double dTime, double dCurrentTime,
		double dStepSize, double dVal);
#ifdef __cplusplus
}
#endif // __cplusplus


/*
 *     Classes declaration
 */

class CISLOMNetIO
{
public:
	CISLOMNetIO(isl::CData * cData) {
		m_dTime = 0.0;
		m_dValue = 0.0;
		m_bIsModified = false;
		m_cData = cData;
	}

	~CISLOMNetIO() {
		m_cData = 0;
	}

	isl::CData * GetData() { return m_cData; }

	void SetTime(double dTime) { m_dTime = dTime; }
	double GetTime() { return m_dTime; }

	void SetValue(double dValue) { m_dValue = dValue; }
	double GetValue() { return m_dValue; }

	void SetModified(bool bVal) { m_bIsModified = bVal; }
	bool IsModified() { return m_bIsModified; }

private:
	double m_dTime;
	double m_dValue;
	bool m_bIsModified;

	isl::CData * m_cData;
};

class CISLOMNetModule
{
public:
	CISLOMNetModule(double dStartTime, double dEndTime, double dStepSize) {
		m_nStatus = 0;
		m_dStartTime = dStartTime;
		m_dEndTime = dEndTime;
		m_dStepSize = dStepSize;
	}

	~CISLOMNetModule() {
		Terminate();
		if (m_mIns.empty() == false) {
			std::map<std::string, CISLOMNetIO *>::iterator it;
			for (it = m_mIns.begin(); it != m_mIns.end(); it++) {
				delete it->second;
			}
			m_mIns.clear();
		}
		if (m_mOuts.empty() == false) {
			std::map<std::string, CISLOMNetIO *>::iterator it;
			for (it = m_mOuts.begin(); it != m_mOuts.end(); it++) {
				delete it->second;
			}
			m_mOuts.clear();
		}
	}

	bool Init(const char * sProcessName, const char * sSessionId) {
		m_nStatus = 0;
		ISLInitialize(sProcessName, sSessionId, m_dStartTime,
			m_dEndTime, m_dStepSize, m_nStatus);
		if (m_nStatus != 0) {
			return false;
		}
		m_sName = std::string(sProcessName);
		m_sSessionId = std::string(sSessionId);
		return true;
	}

	bool Connect() {
		if (ISLConnect(m_sName.c_str(), m_sSessionId.c_str()) != 0) {
			return false;
		}
		return true;
	}

	bool Terminate() {
		m_nStatus = ISLTerminate(m_sName.c_str(), m_sSessionId.c_str());
		if (m_nStatus != 0) {
			return false;
		}
		return true;
	}

	bool AddIO(const char * sId, const char * sConnectId,
		double dInitVal, bool bIsInput) {
		m_nStatus = 0;
		isl::CData * cData = ISLAddRealIO(m_sName.c_str(), m_sSessionId.c_str(), sId,
			sConnectId, dInitVal, bIsInput, m_nStatus);
		if (m_nStatus != 0) {
			return false;
		}
		if (cData != 0) {
			CISLOMNetIO * cIO = new CISLOMNetIO(cData);
			cIO->SetValue(dInitVal);
			if (bIsInput) {
				m_mIns[sId] = cIO;
			}
			else {
				cIO->SetTime(-1);
				m_mOuts[sId] = cIO;
			}
			return true;
		}
		return false;
	}

	double GetStepSize() { return m_dStepSize; }

	CISLOMNetIO * GetIn(const char * sId) {
		try {
			return m_mIns[sId];
		}
		catch (...) {
			return 0;
		}
	}

	CISLOMNetIO * GetOut(const char * sId) {
		try {
			return m_mOuts[sId];
		}
		catch (...) {
			return 0;
		}
	}

	bool ReadIns(double dTime) {
		bool bRet = true;
		std::map<std::string, CISLOMNetIO *>::iterator it;
		for (it = m_mIns.begin(); it != m_mIns.end(); it++) {
			double dVal = 0.0;
			double dRetTime = dTime;
			if (ISLGetReal(it->second->GetData(), dTime, dVal, dRetTime) == 0) {
				if (dVal != it->second->GetValue()) {
					it->second->SetModified(true);
				}
				it->second->SetTime(dRetTime);
				it->second->SetValue(dVal);
			}
			else {
				bRet = false;
			}
		}
		return bRet;
	}

	bool SetAndSendOut(const char * sId, double dTime, double dVal) {
		if (ISLSetReal(m_mOuts[sId]->GetData(), dTime, dVal) == 0) {
			m_mOuts[sId]->SetTime(dTime);
			m_mOuts[sId]->SetValue(dVal);
			return true;
		}
		return false;
	}

	bool CheckAndSendOuts(double dTime) {
		bool bRet = true;
		std::map<std::string, CISLOMNetIO *>::iterator it;
		for (it = m_mOuts.begin(); it != m_mOuts.end(); it++) {
			int nStatus = ISLCheckAndSetReal(it->second->GetData(),
				it->second->GetTime(), dTime, m_dStepSize, it->second->GetValue());
			if (nStatus == 0) {
				it->second->SetTime(dTime);
				bRet = true;
			}
			else if (nStatus == -1) {
				bRet = true;
			}
			else {
				bRet = false;
			}
		}
		return bRet;
	}

private:
	std::string m_sName;
	std::string m_sSessionId;
	int m_nStatus;

	double m_dStartTime;
	double m_dEndTime;
	double m_dStepSize;

	std::map<std::string, CISLOMNetIO *> m_mIns;
	std::map<std::string, CISLOMNetIO *> m_mOuts;
};

#endif // _ISL_OMNETPP_H_
