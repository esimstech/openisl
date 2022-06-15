/*
 *     Name: isl_api_global.h
 *
 *     Description: ISL API DLL macros.
 *
 *     Modification date: 04/03/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_API_GLOBAL_H_
#define _ISL_API_GLOBAL_H_

/*
 *     Macros and constants definition
 */

#ifdef WIN32
#ifdef ISL_API_LIB
#define ISL_API_EXPORT __declspec(dllexport)
#else
#define ISL_API_EXPORT __declspec(dllimport)
#endif
#else // Not WIN32
#define ISL_API_EXPORT
#endif // WIN32

#endif // _ISL_API_GLOBAL_H_
