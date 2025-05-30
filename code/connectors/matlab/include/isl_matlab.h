/* 
 *     Name: isl_matlab.h
 *
 *     Description: Matlab coupling for ISL.
 *
 *     Author: T. Roudier
 *     Copyright (c) 2019-2025 E-Sim Solutions Inc
 *
 *     Distributed under the MIT License.
 *
 *     --------------------------------------------------------------------------
 *
 *     Permission is hereby granted, free of charge, to any person obtaining a
 *     copy of this software and associated documentation files (the �Software�),
 *     to deal in the Software without restriction, including without limitation
 *     the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *     and/or sell copies of the Software, and to permit persons to whom the
 *     Software is furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included in
 *     all copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED �AS IS�, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
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

#ifndef _ISL_MATLAB_H_
#define _ISL_MATLAB_H_

/*
 *     Header files
 */

#include <isl_matlab_global.h>


/*
 *     Functions declaration
 */

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

	ISL_MATLAB_EXPORT int ISLStatusCode();
	ISL_MATLAB_EXPORT bool ISLSetup(const char * sXMLFile);
	ISL_MATLAB_EXPORT bool ISLGetData(const char * sName, void * pData, double * dOutTime, double dTime, bool bWait);
	ISL_MATLAB_EXPORT bool ISLGetDataLast(const char * sName, void * pData, double * dOutTime, bool bWait);
	ISL_MATLAB_EXPORT bool ISLGetDataEvt(const char * sName, void * pData, bool bWait);
	ISL_MATLAB_EXPORT bool ISLSetData(const char * sName, void * pData, double dTime, bool bWait);
	ISL_MATLAB_EXPORT bool ISLSetDataEvt(const char * sName, void * pData, bool bWait);
	ISL_MATLAB_EXPORT bool ISLTerminate();

#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _ISL_MATLAB_H_
