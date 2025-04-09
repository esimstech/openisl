/* 
 *     Name: isl_simulink.cpp
 *
 *     Description: Simulink variables coupling for ISL.
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

#ifndef WIN32
#include <string.h>
#endif
#include "isl_simulink.h"


/*
 *     Variables definition
 */

ISL_SIMULINK_EXPORT isl::CConnect * g_cConnect = NULL;
ISL_SIMULINK_EXPORT bool g_bDoNotCosim = false;


/*
 *     Functions definition
 */

ISL_SIMULINK_EXPORT std::string GetBlockNameFromPath(const char * sPath)
{
	size_t i, j, n;
	// Get the last occurrence of '/'
	n = strlen(sPath);
	i = n;
	while (sPath[i--] != '/');
	// Get the second last occurrence of '/'
	j = i;
	while (sPath[j--] != '/');
	// Erase wrong portions of the string
	std::string sName(sPath);
	sName.erase(i + 1, n - i - 1);
	sName.erase(0, j + 2);
	return sName;
}
