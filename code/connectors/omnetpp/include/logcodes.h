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
	ERROR_CREATEMODEL_FAILED = 1000,
	ERROR_CONNECT_NOMODEL,
	ERROR_ADDREALIO_NOMODEL,
	ERROR_SETSESSIONID_FAILED,
	ERROR_CHECKMODEL_FAILED,
	ERROR_CREATESESSION_FAILED,
	ERROR_NEWIO_FAILED,
	ERROR_GETREAL_FAILED,
	ERROR_SETREAL_FAILED,
	ERROR_GET_NOACCESSTODATA,
	ERROR_SET_NOACCESSTODATA,
	ERROR_CONNECT_FAILED,
	ERROR_DISCONNECT_FAILED
};

// Warning codes
enum {
	WARNING_SENDSTOPEVENT = 1300,
	WARNING_GET_ENDOFSIM_REACHED,
	WARNING_SET_ENDOFSIM_REACHED
};

// Info codes
enum {
	INFO_MODEL_ALREADY_INSTANTIATED = 1500,
	INFO_MODEL_CREATED,
	INFO_SESSION_CREATED,
	INFO_CONNECTEDTO_SESSION,
	INFO_ISLAPI_INITIALIZED,
	INFO_DISCONNECTED,
	INFO_ISLAPI_CLOSING
};

#endif // _LOGCODES_H_