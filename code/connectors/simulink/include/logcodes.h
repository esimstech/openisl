/*
 *     Name: logcodes.h
 *
 *     Description: ISL log codes.
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

#ifndef _LOGCODES_H_
#define _LOGCODES_H_

 /*
  *     Codes definition
  */

// Error codes
enum {
	ERROR_LOADXML_FAILED = 1000,
	ERROR_CHECK_FAILED,
	ERROR_SETNBOUTPUTS_FAILED,
	ERROR_SETNBINPUTS_FAILED,
	ERROR_CREATESESSION_FAILED,
	ERROR_CONNECT_FAILED,
	ERROR_VARIABLE_NOT_DEFINED,
	ERROR_DISCONNECT
};

// Warning codes
enum {
	WARNING_NOIOSTOCONNECT = 1300,
	WARNING_LISTENTOEXITSESSION,
	WARNING_SENDSTOPEVENT
};

// Info codes
enum {
	INFO_XML_LOADED = 1500,
	INFO_SIMULATION_MODE,
	INFO_SIMULATION_STATUS,
	INFO_SESSION_CREATED,
	INFO_CONNECTEDTO_SESSION,
	INFO_ISLAPI_INITIALIZED,
	INFO_STOPREQUESTED,
	INFO_IDENTIFY_VARIABLE,
	INFO_DONOTCONNECT_ISL,
	INFO_DISCONNECTED
};

// Debug codes
enum {
	DEBUG_CONNECTOR_STEP = 2000,
	DEBUG_CONNECTOR_WRITE_DATA,
	DEBUG_CONNECTOR_WRITE_DONE,
	DEBUG_CONNECTOR_READ_DATA,
	DEBUG_CONNECTOR_READ_DONE,
	DEBUG_OUTPUT_STEP,
	DEBUG_OUTPUT_WRITE_DATA,
	DEBUG_OUTPUT_WRITE_DONE,
	DEBUG_INPUT_READ_DATA,
	DEBUG_INPUT_READ_DONE
};

#endif // _LOGCODES_H_