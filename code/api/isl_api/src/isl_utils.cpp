/*
 *     Name: isl_utils.cpp
 *
 *     Description: ISL utilities.
 *
 *     Modification date: 22/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

/*
 *     Header files
 */

#include <stdarg.h>

#include <isl_log.h>
#include "isl_api.h"

/*
 *     Classes definition
 */

std::string isl::CUtils::GetLogFile()
{
	return AppLog->GetLogFile();
}

void isl::CUtils::Info(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogInfo(uId, &sVec[0]);
}

void isl::CUtils::Warning(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogWarning(uId, &sVec[0]);
}

void isl::CUtils::Error(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogError(uId, &sVec[0]);
}

void isl::CUtils::Debug(unsigned int uLevel, unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	AppLogDebug(uLevel, uId, &sVec[0]);
}
