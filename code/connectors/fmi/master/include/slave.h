/*
 *     Name: slave.h
 *
 *     Description: Slave generic class.
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

#ifndef _SLAVE_H_
#define _SLAVE_H_

/*
 *     Header files
 */

#include <string>
#include <boost/dll.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

#include <isl_sem.h>
#include <isl_xml.h>
#include <isl_api.h>


/*
 *     Classes declaration
 */

class CModel;

/*
 *     Class CGenericSlave
 */

class CGenericSlave
{
public:
	enum tStatus {
		SLAVE_STATUS_ERROR = 0,
		SLAVE_STATUS_IDLE,
		SLAVE_STATUS_LIBRARY_LOADED,
		SLAVE_STATUS_FCTS_MAPPED,
		SLAVE_STATUS_INSTANTIATED,
		SLAVE_STATUS_ENTERINITIALIZATION,
		SLAVE_STATUS_INITIALIZED,
		SLAVE_STATUS_RUNNING,
		SLAVE_STATUS_TERMINATED,
		SLAVE_STATUS_RELEASED
	};

	enum tError {
		SLAVE_ERROR_UNKNOWN = -500,
		SLAVE_ERROR_NULL_MODEL,
		SLAVE_ERROR_NULL_FMU,
		SLAVE_ERROR_NO_LIBRARY,
		SLAVE_ERROR_LIBRARY_ALREADY_LOADED,
		SLAVE_ERROR_LIBRARY_NOT_LOADED,
		SLAVE_ERROR_LIBRARY_NOT_EXIST,
		SLAVE_ERROR_LIBRARY_LOAD_FAILED,
		SLAVE_ERROR_INSTANTIATE_FAILED,
		SLAVE_ERROR_NO_COMPONENT,
		SLAVE_ERROR_ENTERINITIALIZATION_FAILED,
		SLAVE_ERROR_INITIALIZED_FAILED,
		SLAVE_ERROR_NO_FMU,
		SLAVE_ERROR_NO_BLACKBOX,
		SLAVE_ERROR_FAILED_DEBUGLOGGING,
		SLAVE_ERROR_FAILED_SETUPEXPERIMENT,
		SLAVE_ERROR_FAILED_ENTERINIT,
		SLAVE_ERROR_FAILED_EXITINIT,
		SLAVE_ERROR_DOSTEP_FAILED,
		SLAVE_ERROR_ASYNCSIM_FAILED,
		SLAVE_ERROR_STOP_REQUIRED,
		SLAVE_ERROR_SIMULATION_FAILED,
		SLAVE_ERROR_DOSTEP_TIMEOUT_REACHED,
		SLAVE_ERROR_SETDATA_FAILED,
		SLAVE_ERROR_GETDATA_FAILED,
		SLAVE_ERROR_SETREAL_FAILED,
		SLAVE_ERROR_GETREAL_FAILED,
		SLAVE_ERROR_SETINTEGER_FAILED,
		SLAVE_ERROR_GETINTEGER_FAILED,
		SLAVE_ERROR_SETBOOLEAN_FAILED,
		SLAVE_ERROR_GETBOOLEAN_FAILED,
		SLAVE_ERROR_SETSTRING_FAILED,
		SLAVE_ERROR_GETSTRING_FAILED,
		SLAVE_ERROR_NOTFORCOSIMULATION,
		SLAVE_ERROR_FMUXML_E1,
		SLAVE_ERROR_FMUXML_E2,
		SLAVE_ERROR_FMUXML_E3,
		SLAVE_ERROR_FMUXML_E4,
		SLAVE_ERROR_FMUXML_E5,
		SLAVE_ERROR_FMUXML_E6,
		SLAVE_ERROR_FMUXML_E7,
		SLAVE_ERROR_FMUXML_E8,
		SLAVE_ERROR_FMUXML_E9
	};

	CGenericSlave();
	~CGenericSlave();

	bool LoadLibrary();
	bool FreeLibrary();
	// Function name may change
	bool HasFunction(std::string & sFunction, bool bPrefix);

	virtual bool MapFunctions() = 0;

	virtual bool Instantiate() = 0;
	virtual bool Initialize() = 0;
	virtual bool EndInitialize() = 0;

	virtual bool DoStep(double dTime, double dTimeStep, bool noSetState = true) = 0;
	virtual bool SetReal(unsigned int uRef, double * dVal) = 0;
	virtual bool GetReal(unsigned int uRef, double * dVal) = 0;
	virtual bool SetInteger(unsigned int uRef, int * nVal) = 0;
	virtual bool GetInteger(unsigned int uRef, int * nVal) = 0;
	virtual bool SetBoolean(unsigned int uRef, bool * bVal) = 0;
	virtual bool GetBoolean(unsigned int uRef, bool * bVal) = 0;
	virtual bool SetString(unsigned int uRef, char * sVal) = 0;
	virtual bool GetString(unsigned int uRef, char * sVal) = 0;

	virtual bool Terminate() = 0;
	virtual bool Reset() = 0;
	virtual bool Free() = 0;

	bool IsVisible();
	void SetVisible(bool bVal);

	tStatus GetStatus();
	void SetStatus(tStatus eStatus);

	tError GetErrorCode();
	std::string GetErrorString();
	void SetErrorCode(tError eVal);

	CModel * GetModel();
	bool SetModel(CModel * cModel);

	void * GetComponent();

	std::string GetLibraryName();

	bool Lock();
	void Unlock();

protected:
	CModel * m_cModel;
	isl::CXML * m_cFMU;
	isl::CConnect * m_cBlackBox;

	void * m_cComponent;
	int m_nCategories;
	char ** m_lsCategories;

	boost::interprocess::interprocess_semaphore m_bsWaitStep;
	int m_nWaitStepTimeout;

	boost::dll::shared_library * m_bdLibrary;

	bool m_bVisible;

	tStatus m_eStatus;
	tError m_eErrorCode;
	std::string m_sError;
};

#endif // _SLAVE_H_
