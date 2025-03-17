/*
 *     Name: appsettings.h
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

#ifndef _ISL_SETTINGS_H_
#define _ISL_SETTINGS_H_

/*
 *     Header files
 */

#include <string>
#include <map>


/*
 *     Classes declaration
 */

namespace isl {
	class CINI;

	class ISL_API_EXPORT CAppSettings
	{
	public:
		typedef enum {
			AS_GRP_COMMON = 0,
			AS_GRP_UNKNOWN = 50
		} tGroup;

		typedef enum {
			AS_CMN_MAXSHMSTRINGSIZE = 0,
			AS_CMN_MAXFIFODEPTH,
			AS_CMN_MAXNBREADERS,
			AS_CMN_MAXNBRUNSIMS,
			AS_CMN_STEPTOLERANCE,
			AS_CMN_ISLCOMPATIBLE,
			AS_CMN_ISGLOBALIPC,
			AS_KEY_UNKNOWN = 500
		} tKey;

		static const std::string c_sFile;

		CAppSettings();
		~CAppSettings();

		bool GetBoolValue(tGroup eGroup, tKey eKey, bool bDefault = false);
		int GetIntValue(tGroup eGroup, tKey eKey, int nDefault = 0);
		double GetDoubleValue(tGroup eGroup, tKey eKey, double dDefault = 0.0);
		std::string GetStringValue(tGroup eGroup, tKey eKey,
			const std::string & sDefault = "", bool bResolve = false);

		// Get values of known parameters
		unsigned int GetMaxSHMStringSize();
		unsigned short GetMaxFIFODepth();
		int GetMaxNbReaders();
		int GetMaxNbRunSims();
		double GetStepTolerance();
		bool IsISLCompatible();
		bool IsGlobalIPC();

	protected:
		std::map<unsigned int, std::string> m_mGroupNames;
		std::map<unsigned int, std::string> m_mKeyNames;

	private:
		CINI * m_cProperties;
		bool m_bLoaded;
	};
}

#endif // _ISL_SETTINGS_H_