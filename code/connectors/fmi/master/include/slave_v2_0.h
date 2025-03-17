/* 
 *     Name: slave_v2_0.h
 *
 *     Description: Class CSlave2_0: FMU 2.0 slave model.
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

#ifndef _SLAVE_V2_0_H_
#define _SLAVE_V2_0_H_

/*
 *     Header files
 */

#include "slave.h"


/*
 *     Classes declaration
 */

/*
 *     Class CFMUSlaveV2
 */

class CSlave2_0 : public CGenericSlave
{
public:
	CSlave2_0();
	~CSlave2_0();

	bool MapFunctions();

	bool Instantiate();
	bool Initialize();
	bool EndInitialize();

	bool DoStep(double dTime, double dTimeStep, bool noSetState = true);
	bool SetReal(unsigned int uRef, double * dVal);
	bool GetReal(unsigned int uRef, double * dVal);
	bool SetInteger(unsigned int uRef, int * nVal);
	bool GetInteger(unsigned int uRef, int * nVal);
	bool SetBoolean(unsigned int uRef, bool * bVal);
	bool GetBoolean(unsigned int uRef, bool * bVal);
	bool SetString(unsigned int uRef, char * sVal);
	bool GetString(unsigned int uRef, char * sVal);

	bool Terminate();
	bool Reset();
	bool Free();
};

#endif // _SLAVE_V2_0_H_
