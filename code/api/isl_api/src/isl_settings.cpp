/*
 *     Name: appsettings.cpp
 *
 *     Description: Manage the ISL API settings.
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

#include <isl_api_global.h>
#include <isl_ini.h>
#include <isl_log.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "isl_const.h"
#include "isl_errorcodes.h"
#include "isl_settings.h"


/*
 *     Constants definition
 */

const std::string isl::CAppSettings::c_sFile = "isl_api.ini";


/*
 *     Classes definition
 */

/*
 *     Class CAppSettings
 */

isl::CAppSettings::CAppSettings()
{
	//
	m_mGroupNames[AS_GRP_COMMON] = "Common";
	//
	m_mKeyNames[AS_CMN_MAXSHMSTRINGSIZE] = "MaxSHMStringSize";
	m_mKeyNames[AS_CMN_MAXFIFODEPTH] = "MaxFIFODepth";
	m_mKeyNames[AS_CMN_MAXNBREADERS] = "MaxNbReaders";
	m_mKeyNames[AS_CMN_MAXNBRUNSIMS] = "MaxNbSimulations";
	m_mKeyNames[AS_CMN_STEPTOLERANCE] = "StepTolerance";
	m_mKeyNames[AS_CMN_ISLCOMPATIBLE] = "ISLCompatible";
	m_mKeyNames[AS_CMN_ISGLOBALIPC] = "IsGlobalIPC";
	//
	m_cProperties = new CINI(c_sFile, true);
	m_bLoaded = true;
	if (m_cProperties->Load() == false) {
		AppLogError(APPSETTINGS_INI_LOAD_FAILED, "Failed to load the file: %s, Status: %d",
			m_cProperties->GetFile().c_str(), m_cProperties->GetStatus());
		m_bLoaded = false;
	}
}

isl::CAppSettings::~CAppSettings()
{
	if (m_cProperties != 0) {
		delete m_cProperties;
	}
	m_cProperties = 0;
}

bool isl::CAppSettings::GetBoolValue(tGroup eGroup, tKey eKey, bool bDefault)
{
	std::string sVal = GetStringValue(eGroup, eKey);
	if (sVal.empty()) {
		return bDefault;
	}
	if (sVal == "true") {
		return true;
	}
	if (sVal == "false") {
		return false;
	}
	return bDefault;
}

int isl::CAppSettings::GetIntValue(tGroup eGroup, tKey eKey, int nDefault)
{
	std::string sVal = GetStringValue(eGroup, eKey);
	try {
		return boost::lexical_cast<int>(sVal);
	}
	catch (...) {
		return nDefault;
	}
}

double isl::CAppSettings::GetDoubleValue(tGroup eGroup, tKey eKey, double dDefault)
{
	std::string sVal = GetStringValue(eGroup, eKey);
	try {
		return boost::lexical_cast<double>(sVal);
	}
	catch (...) {
		return dDefault;
	}
}

std::string isl::CAppSettings::GetStringValue(tGroup eGroup, tKey eKey,
	const std::string & sDefault, bool bResolve)
{
	if (m_bLoaded == false) {
		AppLogError(APPSETTINGS_INI_NOT_LOADED, "The file %s has not been loaded.",
			m_cProperties->GetFile().c_str());
		return sDefault;
	}
	if ((eGroup < 0) || (eGroup >= AS_GRP_UNKNOWN) || (m_mGroupNames.count(eGroup) == 0)) {
		AppLogError(APPSETTINGS_UNKNOWN_GROUP, "Unknown group id: %d.", eGroup);
		return sDefault;
	}
	if ((eKey < 0) || (eKey >= AS_KEY_UNKNOWN) || (m_mKeyNames.count(eKey) == 0)) {
		AppLogError(APPSETTINGS_UNKNOWN_KEY, "Unknown key id: %d.", eKey);
		return sDefault;
	}
	std::string sPath = boost::str(boost::format("%1%.%2%") % m_mGroupNames[eGroup] % m_mKeyNames[eKey]);
	std::string sVal = m_cProperties->GetParameter(sPath, bResolve);
	if (sVal.empty()) {
		sVal = sDefault;
	}
	return sVal;
}

unsigned int isl::CAppSettings::GetMaxSHMStringSize()
{
	return (unsigned int)GetIntValue(AS_GRP_COMMON, AS_CMN_MAXSHMSTRINGSIZE, DEFAULT_MAX_SHM_STRING_SIZE);
}

unsigned short isl::CAppSettings::GetMaxFIFODepth()
{
	return (unsigned short)GetIntValue(AS_GRP_COMMON, AS_CMN_MAXFIFODEPTH, DEFAULT_MAX_FIFO_DEPTH);
}

int isl::CAppSettings::GetMaxNbReaders()
{
	return GetIntValue(AS_GRP_COMMON, AS_CMN_MAXNBREADERS, DEFAULT_MAX_NB_READERS);
}

int isl::CAppSettings::GetMaxNbRunSims()
{
	return GetIntValue(AS_GRP_COMMON, AS_CMN_MAXNBRUNSIMS, DEFAULT_MAX_NB_RUN_SIMS);
}

double isl::CAppSettings::GetStepTolerance()
{
	return GetDoubleValue(AS_GRP_COMMON, AS_CMN_STEPTOLERANCE, DEFAULT_STEP_TOLERANCE);
}

bool isl::CAppSettings::IsISLCompatible()
{
	return GetBoolValue(AS_GRP_COMMON, AS_CMN_ISLCOMPATIBLE, false);
}

bool isl::CAppSettings::IsGlobalIPC()
{
	return GetBoolValue(AS_GRP_COMMON, AS_CMN_ISGLOBALIPC, false);
}
