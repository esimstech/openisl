/*
 *     Name: ISLInitialize
 *
 *     Description: Initialize an ISL session.
 *     Syntax: bOk = ISLInitialize(XMLFilename)
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
 *     Macros definition
 */

// Debug ISL
#if 0
#define DEBUG_ISL
#endif


/*
 *     Header files
 */

#include <mex.h>
#include <isl_matlab.h>


/*
 *     Matlab gateway function definition
 */

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
	// Number of input arguments
	if (nrhs != 1) {
		mexErrMsgIdAndTxt("ISL:ISLInitialize:nrhs", "1 input argument required.");
	}
	// Number of output arguments
	if (nlhs != 1) {
		mexErrMsgIdAndTxt("ISL:ISLInitialize:nrhs", "1 output argument required.");
	}
	// Input must be a string
	if (!mxIsChar(prhs[0])) {
		mexErrMsgIdAndTxt("ISL:ISLInitialize:inputNotString", "Input argument must be a string.");
	}
	// Input must be a row vector
	if (mxGetM(prhs[0]) != 1) {
		mexErrMsgIdAndTxt("ISL:ISLInitialize:inputNotVector", "Input argument must be a row vector.");
	}
	// Get FMX and session names
	char * sXMLFile = mxArrayToString(prhs[0]);
	plhs[0] = mxCreateLogicalScalar(false);
	bool * bRet = (bool *)mxGetData(plhs[0]);
	*bRet = ISLSetup(sXMLFile);
}
