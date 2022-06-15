/*
 *     Name: appsettings.cpp
 *
 *     Description: Manage the ISL API settings.
 *
 *     Modification date: 18/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

/*
 *     Header files
 */

#include <isl_api_global.h>
#include <isl_log.h>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

#include "isl_const.h"
#include "isl_errorcodes.h"
#include "appsettings.h"


/*
 *     Constants definition
 */

const char * isl::CAppSettings::c_aGroupNames[CAppSettings::AS_GRP_UNKNOWN] = {
	"Common"
};

const char * isl::CAppSettings::c_aKeyNames[CAppSettings::AS_KEY_UNKNOWN] = {
	"MaxSHMStringSize",
	"MaxFIFODepth",
	"MaxNbReaders",
	"MaxNbSimulations",
	"StepTolerance",
	"ISLCompatible"
};

const std::string isl::CAppSettings::c_sFile = "isl_api.ini";


/*
 *     Classes definition
 */

/*
 *     Class CAppSettings
 */

isl::CAppSettings::CAppSettings()
	: m_cProperties(c_sFile, true)
{
	m_bLoaded = true;
	if (m_cProperties.Load() == false) {
		AppLogError(APPSETTINGS_INI_LOAD_FAILED, "Failed to load the file: %s, Status: %d",
			m_cProperties.GetFile().c_str(), m_cProperties.GetStatus());
		m_bLoaded = false;
	}
}

isl::CAppSettings::~CAppSettings()
{
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

std::string isl::CAppSettings::GetStringValue(tGroup eGroup, tKey eKey, const std::string & sDefault)
{
	if (m_bLoaded == false) {
		AppLogError(APPSETTINGS_INI_NOT_LOADED, "The file %s has not been loaded.",
			m_cProperties.GetFile().c_str());
		return sDefault;
	}
	if ((eGroup < 0) && (eGroup >= AS_GRP_UNKNOWN)) {
		AppLogError(APPSETTINGS_UNKNOWN_GROUP, "Unknown group id: %d.", eGroup);
		return sDefault;
	}
	if ((eKey < 0) && (eKey >= AS_KEY_UNKNOWN)) {
		AppLogError(APPSETTINGS_UNKNOWN_KEY, "Unknown key id: %d.", eKey);
		return sDefault;
	}
	std::string sPath = boost::str(boost::format("%1%.%2%") % c_aGroupNames[eGroup] % c_aKeyNames[eKey]);
	std::string sVal = m_cProperties.GetParameter(sPath);
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
