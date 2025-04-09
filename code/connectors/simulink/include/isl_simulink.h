/* 
 *     Name: isl_simulink.h
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

#ifndef _ISL_SIMULINK_H_
#define _ISL_SIMULINK_H_

/*
 *     Header files
 */

#include <isl_api.h>
#include <isl_simulink_global.h>


/*
 *     Types definition
 */

typedef struct {
	double m_dFixedStep;
	double m_dStep;
	double m_dInitialStep;
	double m_dLastTime;
	isl::CData * m_cData;
} tISLDataInfo;


/*
 *     Variables declaration
 */

extern ISL_SIMULINK_EXPORT isl::CConnect * g_cConnect;
extern ISL_SIMULINK_EXPORT bool g_bDoNotCosim;


/*
 *     Functions declaration
 */

ISL_SIMULINK_EXPORT std::string GetBlockNameFromPath(const char * sPath);

#endif // _ISL_SIMULINK_H_
