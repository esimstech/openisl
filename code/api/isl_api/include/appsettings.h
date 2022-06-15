/*
 *     Name: appsettings.h
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

#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

/*
 *     Header files
 */

#include <isl_ini.h>


/*
 *     Classes declaration
 */

namespace isl {
	class CAppSettings
	{
	public:
		typedef enum {
			AS_GRP_COMMON = 0,
			AS_GRP_UNKNOWN
		} tGroup;

		typedef enum {
			AS_CMN_MAXSHMSTRINGSIZE = 0,
			AS_CMN_MAXFIFODEPTH,
			AS_CMN_MAXNBREADERS,
			AS_CMN_MAXNBRUNSIMS,
			AS_CMN_STEPTOLERANCE,
			AS_CMN_ISLCOMPATIBLE,
			AS_KEY_UNKNOWN
		} tKey;

		static const char * c_aGroupNames[AS_GRP_UNKNOWN];
		static const char * c_aKeyNames[AS_KEY_UNKNOWN];

		static const std::string c_sFile;

		CAppSettings();
		~CAppSettings();

		bool GetBoolValue(tGroup eGroup, tKey eKey, bool bDefault = false);
		int GetIntValue(tGroup eGroup, tKey eKey, int nDefault = 0);
		double GetDoubleValue(tGroup eGroup, tKey eKey, double dDefault = 0.0);
		std::string GetStringValue(tGroup eGroup, tKey eKey, const std::string & sDefault = "");

		// Get values of known parameters
		unsigned int GetMaxSHMStringSize();
		unsigned short GetMaxFIFODepth();
		int GetMaxNbReaders();
		int GetMaxNbRunSims();
		double GetStepTolerance();
		bool IsISLCompatible();

	private:
		CINI m_cProperties;
		bool m_bLoaded;
	};
}

#endif // _APPSETTINGS_H_