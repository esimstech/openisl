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

// Status codes
typedef enum {
	ISL_MAT_SC_SUCCESS = 0,
	ISL_MAT_SC_XMLFILE,
	ISL_MAT_SC_LOADXML,
	ISL_MAT_SC_CREATESESSION,
	ISL_MAT_SC_CONNECTSESSION,
	ISL_MAT_SC_DATAID,
	ISL_MAT_SC_MAPDATA,
	ISL_MAT_SC_GETDATA,
	ISL_MAT_SC_SETDATA,
	ISL_MAT_SC_IOMAP,
	ISL_MAT_SC_TERMINATED,
	ISL_MAT_SC_NOTRECEIVED,
	ISL_MAT_SC_NOTSENT,
	ISL_MAT_SC_NOSESSION,
	ISL_MAT_SC_DISCONNECT
} tStatusCode;

// Error codes
enum {
	ERROR_LOADXML_FAILED = 1000,
	ERROR_CREATESESSION_FAILED,
	ERROR_CONNECT_FAILED,
	ERROR_DISCONNECT
};

// Warning codes
enum {
	WARNING_LISTENTOEXITSESSION = 1300,
	WARNING_SENDSTOPEVENT
};

// Info codes
enum {
	INFO_XML_LOADED = 1500,
	INFO_SESSION_CREATED,
	INFO_CONNECTEDTO_SESSION,
	INFO_ISLAPI_INITIALIZED,
	INFO_DISCONNECTED,
	INFO_ISLAPI_CLOSING
};

// Debug codes
enum {
	DEBUG_GETREAL_TIME_DATA = 2000,//unused
	DEBUG_SETREAL_TIME_DATA//unused
};

#endif // _LOGCODES_H_