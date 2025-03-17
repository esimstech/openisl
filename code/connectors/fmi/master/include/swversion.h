/*
 *     Name: swversion.h
 *
 *     Description: Application version numbers.
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

#ifndef _SWVERSION_H_
#define _SWVERSION_H_

/*
 *     Constants and macros definition
 */

#define APP_NAME				"OpenISL FMI Master Simulator"
#define APP_SHORT_NAME			"islmasterfmi"

#ifndef MAJOR_VERSION_NUMBER
#define MAJOR_VERSION_NUMBER	1
#endif // MAJOR_VERSION_NUMBER
#ifndef MINOR_VERSION_NUMBER
#define MINOR_VERSION_NUMBER	0
#endif // MINOR_VERSION_NUMBER
#ifndef PATCH_VERSION_NUMBER
#define PATCH_VERSION_NUMBER	0
#endif // PATCH_VERSION_NUMBER
#ifndef BUILD_VERSION_NUMBER
#define BUILD_VERSION_NUMBER	2
#endif // BUILD_VERSION_NUMBER
#define BUILD_STATE				-1  // Can be A<n> (alpha), B<n> (beta), RC<n> (Release Candidate), or -1
// or -1 (nothing)

#if defined(WIN64)
#define PLATFORM_VERSION		"64-bit"
#elif defined(WIN32)
#define PLATFORM_VERSION		"32-bit"
#else
#define PLATFORM_VERSION		""
#endif

#if (BUILD_STATE==-1)
#define FULL_VERSION_NUMBER		MAJOR_VERSION_NUMBER.MINOR_VERSION_NUMBER.PATCH_VERSION_NUMBER.BUILD_VERSION_NUMBER
#else // BUILD_STATE
#define FULL_VERSION_NUMBER		MAJOR_VERSION_NUMBER.MINOR_VERSION_NUMBER.PATCH_VERSION_NUMBER.BUILD_VERSION_NUMBER BUILD_STATE
#endif // BUILD_STATE

#define VERSION_NUMBER			FULL_VERSION_NUMBER

#define TRANSLATE_TOSTRING(x)	#x
#define TOSTRING(x)				TRANSLATE_TOSTRING(x)

#define GET_APP_NAME(x)			APP_NAME " " TRANSLATE_TOSTRING(x)
#define GET_APP_VERSION(x)		TRANSLATE_TOSTRING(x)

#define APP_DESC				"FMI master simulator connected to the OpenISL API."

#endif // _SWVERSION_H_
