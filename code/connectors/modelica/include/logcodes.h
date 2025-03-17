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
	ERROR_CREATESESSION_FAILED,
	ERROR_CONNECT_FAILED,
	ERROR_GETREAL_DISCONNECT,
	ERROR_SETREAL_DISCONNECT
};

// Warning codes
enum {
	WARNING_WRONG_SESSIONID = 1300,
	WARNING_LISTENTOEXITSESSION,
	WARNING_GETREAL_SENDSTOPEVENT,
	WARNING_SETREAL_SENDSTOPEVENT
};

// Info codes
enum {
	INFO_XML_LOADED = 1500,
	INFO_SESSIONID_SET,
	INFO_SESSION_CREATED,
	INFO_CONNECTEDTO_SESSION,
	INFO_ISLAPI_INITIALIZED,
	INFO_GETREAL_DISCONNECTED,
	INFO_GETREAL_ISLAPI_CLOSING,
	INFO_SETREAL_DISCONNECTED,
	INFO_SETREAL_ISLAPI_CLOSING
};

// Debug codes
enum {
	DEBUG_GETREAL_TIME_DATA = 2000,
	DEBUG_SETREAL_TIME_DATA
};

#endif // _LOGCODES_H_