/*
 *     Name: tts_misc.h
 *
 *     Description: Misc functions.
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
		static std::string GetRuntimePath();
	};

	class CFileSystem {
	public:
		static int CreateEmptyFile(const std::string & sFileName);
	};
}

#endif // _ISL_MISC_H_