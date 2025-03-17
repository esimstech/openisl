/*
 *     Name: slave.cpp
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

/*
 *     Header files
 */

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/format.hpp>
#include <boost/filesystem.hpp>

#include <isl_misc.h>
#include <isl_log.h>

#include "fmusim_const.h"
#include "appsettings.h"
#include "slave.h"
#include "model.h"


/*
 *     Classes definition
 */

/*
 *     Class CGenericSlave
 */

CGenericSlave::CGenericSlave() : m_bsWaitStep(0)
{
	m_eStatus = SLAVE_STATUS_IDLE;
	m_eErrorCode = SLAVE_ERROR_UNKNOWN;
	m_cModel = 0;
	m_cFMU = 0;
	m_cBlackBox = 0;
	m_cComponent = 0;
	m_nCategories = 0;
	m_lsCategories = 0;
	m_nWaitStepTimeout = CExeSettings().GetTimeOutPendingStep();
	m_bdLibrary = 0;
	m_bVisible = false;
}

CGenericSlave::~CGenericSlave()
{
	AppLogInfo(INFO_SLAVE_DISCONNECT, "Disconnecting the slave instance.");
	if (m_bdLibrary != 0) {
		if (m_bdLibrary->is_loaded()) {
			m_bdLibrary->unload();
		}
		delete m_bdLibrary;
	}
	m_bdLibrary = 0;
	m_cModel = 0;
	m_cFMU = 0;
	m_cBlackBox = 0;
	m_cComponent = 0;
	if (m_lsCategories != 0) {
		if (m_nCategories > 0) {
			for (int i = 0; i < m_nCategories; i++) {
				free(m_lsCategories[i]);
			}
		}
		free(m_lsCategories);
	}
	m_nCategories = 0;
	m_lsCategories = 0;
}

bool CGenericSlave::LoadLibrary()
{
	if (m_cModel == 0) {
		m_eErrorCode = SLAVE_ERROR_NULL_MODEL;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	std::string sModelPath = m_cModel->GetDir();
	if (m_cFMU == 0) {
		m_eErrorCode = SLAVE_ERROR_NULL_FMU;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Library file
	try {
		if (m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetNode("CoSimulation") == 0) {
			m_eErrorCode = SLAVE_ERROR_NOTFORCOSIMULATION;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E1;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	std::string sModelIdentifier;
	try {
		sModelIdentifier = m_cFMU->GetRoot()->GetNode("fmiModelDescription")
			->GetNode("CoSimulation")->GetAttribute("modelIdentifier");
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E2;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	std::string sLibName = boost::str(boost::format(FMU_LIB_PATH) % sModelPath % sModelIdentifier);
	if (boost::filesystem::exists(sLibName) == false) {
		m_eErrorCode = SLAVE_ERROR_LIBRARY_NOT_EXIST;
		m_sError = boost::str(boost::format("The library file '%1%' doesn't exist.") % sLibName);
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Add the library path to the search path in any case of dependencies
	// with additional dynamic libraries
	boost::filesystem::path bfpLib(sLibName);
	isl::CApplication::SetLibraryDirectory(bfpLib.parent_path().string(), true);
	// Load the library
	if (m_bdLibrary != 0) {
		if (m_bdLibrary->is_loaded()) {
			m_eErrorCode = SLAVE_ERROR_LIBRARY_ALREADY_LOADED;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
		delete m_bdLibrary;
		m_bdLibrary = 0;
	}
	m_bdLibrary = new boost::dll::shared_library();
	try {
		m_bdLibrary->load(bfpLib);
	}
	catch (const std::exception& e) {
		m_eErrorCode = SLAVE_ERROR_LIBRARY_LOAD_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		m_sError = boost::str(boost::format("Error from boost: %1%") % e.what());
		delete m_bdLibrary;
		m_bdLibrary = 0;
		return false;
	}
	if (m_bdLibrary->is_loaded() == false) {
		m_eErrorCode = SLAVE_ERROR_LIBRARY_NOT_LOADED;
		m_eStatus = SLAVE_STATUS_ERROR;
		delete m_bdLibrary;
		m_bdLibrary = 0;
		return false;
	}
	//
	m_eStatus = SLAVE_STATUS_LIBRARY_LOADED;
	return true;
}

bool CGenericSlave::FreeLibrary()
{
	isl::CApplication::SetLibraryDirectory("", true);
	if (m_bdLibrary != NULL) {
		if (m_bdLibrary->is_loaded()) {
			m_bdLibrary->unload();
		}
		delete m_bdLibrary;
	}
	m_bdLibrary = 0;
	//
	return true;
}

bool CGenericSlave::HasFunction(std::string & sFunction, bool bPrefix)
{
	if (m_cModel == 0) {
		m_eErrorCode = SLAVE_ERROR_NULL_MODEL;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (m_cFMU == 0) {
		m_eErrorCode = SLAVE_ERROR_NULL_FMU;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (m_bdLibrary == 0) {
		m_eErrorCode = SLAVE_ERROR_NO_LIBRARY;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (m_bdLibrary->is_loaded() == false) {
		m_eErrorCode = SLAVE_ERROR_LIBRARY_NOT_LOADED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	std::string sName;
	if (bPrefix) {
		try {
			if (m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetNode("CoSimulation") == 0) {
				m_eErrorCode = SLAVE_ERROR_NOTFORCOSIMULATION;
				m_eStatus = SLAVE_STATUS_ERROR;
				return false;
			}
		}
		catch (...) {
			m_eErrorCode = SLAVE_ERROR_FMUXML_E3;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
		std::string sModelIdentifier;
		try {
			sModelIdentifier = m_cFMU->GetRoot()->GetNode("fmiModelDescription")
				->GetNode("CoSimulation")->GetAttribute("modelIdentifier");
		}
		catch (...) {
			m_eErrorCode = SLAVE_ERROR_FMUXML_E4;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
		sName = boost::str(boost::format("%1%_") % sModelIdentifier);
	}
	sFunction = sName + sFunction;
	return m_bdLibrary->has(sFunction);
}

bool CGenericSlave::IsVisible()
{
	return m_bVisible;
}

void CGenericSlave::SetVisible(bool bVal)
{
	m_bVisible = bVal;
}

CGenericSlave::tStatus CGenericSlave::GetStatus()
{
	return m_eStatus;
}

void CGenericSlave::SetStatus(tStatus eStatus)
{
	m_eStatus = eStatus;
}

CGenericSlave::tError CGenericSlave::GetErrorCode()
{
	return m_eErrorCode;
}

std::string CGenericSlave::GetErrorString()
{
	return m_sError;
}

void CGenericSlave::SetErrorCode(tError eVal)
{
	m_eErrorCode = eVal;
}

CModel * CGenericSlave::GetModel()
{
	return m_cModel;
}

bool CGenericSlave::SetModel(CModel * cModel)
{
	if (cModel != 0) {
		if (cModel->GetFMU() == 0) {
			m_eErrorCode = SLAVE_ERROR_NO_FMU;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
		if (cModel->GetBlackBox() == 0) {
			m_eErrorCode = SLAVE_ERROR_NO_BLACKBOX;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
		m_cModel = cModel;
		m_cFMU = m_cModel->GetFMU();
		m_cBlackBox = m_cModel->GetBlackBox();
	}
	return false;
}

void * CGenericSlave::GetComponent()
{
	return m_cComponent;
}

std::string CGenericSlave::GetLibraryName()
{
	if (m_bdLibrary != 0) {
		return m_bdLibrary->location().string();
	}
	return std::string();
}

bool CGenericSlave::Lock()
{
	if (m_nWaitStepTimeout > 0) {
		boost::posix_time::ptime
			t(boost::posix_time::microsec_clock::universal_time());
		return m_bsWaitStep.timed_wait(t + boost::posix_time::seconds(m_nWaitStepTimeout));
	}
	m_bsWaitStep.wait();
	return true;
}

void CGenericSlave::Unlock()
{
	m_bsWaitStep.post();
}
