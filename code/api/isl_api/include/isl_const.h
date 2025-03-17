/*
 *     Name: isl_const.h
 *
 *     Description: ISL API constants.
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

#ifndef _ISL_CONST_H_
#define _ISL_CONST_H_

/*
 *     Constants definition
 */

// Default log file name
#ifndef LOG_FILE_NAME
#define LOG_FILE_NAME				"openisl"
#endif

// Default maximum of FIFO depth
#define DEFAULT_MAX_FIFO_DEPTH		1024

// Default maximum number of simulations running
#define DEFAULT_MAX_NB_RUN_SIMS		256

// Default maximum of number of readers
#define DEFAULT_MAX_NB_READERS		16

// Default maximum size of a string (SHM)
#define DEFAULT_MAX_SHM_STRING_SIZE	1024

// Time value for event based exchange
#define EVENT_DEF_TIME_VAL			-1.0

// Default Step Tolerance
#define DEFAULT_STEP_TOLERANCE	1e-6

#endif // _ISL_CONST_H_
