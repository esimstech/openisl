/*
 *     Name: tts_misc.h
 *
 *     Description: Misc functions.
 *
 *     Modification date: 17/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_MISC_H_
#define _ISL_MISC_H_

/*
 *     Header files
 */


#include <string>


/*
 *     Macros and constants definition
 */

#define FILE_DOT_EXT(x)			"." x

#if defined(__GNUC__) || defined(__clang__)
#define DEPRECATED __attribute__((deprecated))
#elif defined(_MSC_VER)
#define DEPRECATED __declspec(deprecated)
#else
#pragma message("WARNING: You need to implement DEPRECATED for this compiler")
#define DEPRECATED
#endif


/*
 *     Classes declaration
 */

namespace isl {
	class CString {
	public:
		static int GetInteger(const std::string & sIn);
		static double GetDouble(const std::string & sIn);

		static unsigned int FromStrToInt(const std::string & sIn);
		static std::string FromIntToStr(unsigned int uIn);

		static int Reset(char * sDest, size_t uSize);
		static int Copy(char * sDest, size_t uSize, const char * sSrc);
		static int CopyN(char * sDest, size_t uSize, const char * sSrc, size_t uN);
		static int Concatenate(char * sDest, size_t uSize, const char * sSrc);

		static std::string ToWord(const std::string & sIn);

		static std::string Format(const char * sFormat, ...);

		static std::string Getsha1(const std::string & sIn);
	};

	class CApplication {
	public:
		static void SetLibraryDirectory(const std::string & sPath, bool bNative);
	};
}

#endif // _ISL_MISC_H_