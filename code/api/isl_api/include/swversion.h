/*
 *     Name: swversion.h
 *
 *     Description: OpenISL version numbers.
 *
 *     Modification date: 04/03/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _SWVERSION_H_
#define _SWVERSION_H_

/*
 *     Constants and macros definition
 */

#define APP_NAME				"OpenISL API"
#define APP_SHORT_NAME			"isl_api"

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
#define BUILD_VERSION_NUMBER	1
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

#define APP_DESC				"A open-source co-simulation programming interface."

#endif // _SWVERSION_H_
