/*
 *     Name: isl_utils.cpp
 *
 *     Description: ISL utilities.
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

/*
 *     Header files
 */

#include <stdarg.h>
#include <boost/filesystem.hpp>
#include <boost/process/environment.hpp>

#include <isl_misc.h>
#include <isl_log.h>
#include <isl_api.h>


/*
 *     Classes definition
 */

std::string isl::CUtils::GetISLPath()
{
	std::string sISLPath = boost::this_process::environment()["OPENISL_PATH"].to_string();
	if (sISLPath.empty()) {
		boost::filesystem::path bpPath(isl::CApplication::GetRuntimePath());
		sISLPath = bpPath.parent_path().parent_path().string();
	}
	return sISLPath;
}

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

void isl::CUtils::CloseLog()
{
	AppLog_Close;
}
