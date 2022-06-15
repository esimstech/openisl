/*
 *     Name: isl_utils.h
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

#ifndef _ISL_UTILS_H_
#define _ISL_UTILS_H_

/*
 *     Header files
 */

#include <string>


/*
 *     Classes declaration
 */

namespace isl {
	class ISL_API_EXPORT CUtils
	{
	public:
		static std::string GetLogFile();

		static void Info(unsigned int uId, const char * sFormat, ...);
		static void Warning(unsigned int uId, const char * sFormat, ...);
		static void Error(unsigned int uId, const char * sFormat, ...);

		static void Debug(unsigned int uLevel, unsigned int uId, const char * sFormat, ...);
	};
}

#define ISLLogInfo		isl::CUtils::Info
#define ISLLogWarning	isl::CUtils::Warning
#define ISLLogError		isl::CUtils::Error
#define ISLLogDebug		isl::CUtils::Debug

#endif // 
