/*
 *     Name: appsettings.h
 *
 *     Description: Manage the application settings.
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

#ifndef _APPSETTINGS_H_
#define _APPSETTINGS_H_

/*
 *     Header files
 */

#include <isl_api_global.h>
#include <isl_settings.h>


/*
 *     Classes declaration
 */

class CExeSettings : public isl::CAppSettings
{
public:
	typedef enum {
		APP_GRP_FMI = 1,
		APP_GRP_UNKNOWN
	} tAppGroup;

	typedef enum {
		APP_FMI_TIMEOUTPENDINGSTEP = 100,
		APP_FMI_ZIPCMD,
		APP_KEY_UNKNOWN
	} tAppKey;

	CExeSettings();
	~CExeSettings();

	// Get values for known parameters
	int GetTimeOutPendingStep();
	std::string GetZipCmd();
};

#endif // _APPSETTINGS_H_