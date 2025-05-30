/*
 *     Name: ISLSetDouble
 *
 *     Description: Set real data to the ISL session.
 *     Syntax: bOk = ISLSetDouble(VarName, dVal, dTime, bWait)
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
	if (nrhs != 4) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:nrhs", "4 input arguments required.");
	}
	// Number of output arguments
	if (nlhs != 1) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:nrhs", "1 output argument required.");
	}
	// Input #1 must be a string
	if (!mxIsChar(prhs[0])) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input1NotString", "Input argument #1 must be a string.");
	}
	// Input #1 must be a row vector
	if (mxGetM(prhs[0]) != 1) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input1NotVector", "Input argument #1 must be a row vector.");
	}
	// Input #2 must be a double
	if (!mxIsDouble(prhs[1])) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input2NotDouble", "Input argument #2 must be a scalar or type double.");
	}
	if ((mxGetM(prhs[1]) != 1) && (mxGetN(prhs[1]) != 1)) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input2NotScalar", "Input argument #2 must be a scalar or type double.");
	}
	// Input #3 must be a double
	if (!mxIsDouble(prhs[2])) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input3NotDouble", "Input argument #3 must be a scalar or type double.");
	}
	if ((mxGetM(prhs[2]) != 1) && (mxGetN(prhs[2]) != 1)) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input3NotScalar", "Input argument #3 must be a scalar or type double.");
	}
	// Input #4 must be a logical
	if (!mxIsLogicalScalar(prhs[3])) {
		mexErrMsgIdAndTxt("ISL:ISLSetDouble:input4NotLogical", "Input argument #4 must be a scalar or type logical.");
	}
	// Get input arguments
	char * sName = mxArrayToString(prhs[0]);
	double * dVal = (double *)mxGetData(prhs[1]);
	double dTime = mxGetScalar(prhs[2]);
	bool * bWait = mxGetLogicals(prhs[3]);
	// Prepare outputs
	plhs[0] = mxCreateLogicalScalar(false);
	bool * bRet = (bool *)mxGetData(plhs[0]);
	*bRet = ISLSetData(sName, dVal, dTime, bWait[0]);
}
