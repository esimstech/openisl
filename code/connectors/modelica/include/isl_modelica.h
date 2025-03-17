/* 
 *     Name: isl_modelica.h
 *
 *     Description: Modelica coupling for OpenISL.
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

#ifndef _ISL_MODELICA_H_
#define _ISL_MODELICA_H_

/*
 *     Header files
 */

#include <isl_modelica_global.h>


/*
 *     Variables declaration
 */

extern ISL_MODELICA_EXPORT void * g_cConnect;


/*
 *     Functions declaration
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	ISL_MODELICA_EXPORT int ISLInitialize(int nISLId, const char * sXMLFile, const char * sSession, const char * sName);
	ISL_MODELICA_EXPORT double ISLGetStep(int nId);
    ISL_MODELICA_EXPORT double ISLCGetReal(int nId, double dTime, int nEnd);
    ISL_MODELICA_EXPORT int ISLCSetReal(int nId, double dVal, double dTime, int nEnd);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _ISL_MODELICA_H_
