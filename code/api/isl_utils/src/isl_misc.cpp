/*
 *     Name: tts_misc.cpp
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

/*
 *     Header files
 */

#ifdef WIN32
#include <windows.h>
#else
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <dlfcn.h>
#endif // WIN32

#include <isl_misc.h>
#include <string>
#include <codecvt>
#include <vector>
#include <cstdarg>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/uuid/detail/sha1.hpp>


/*
 *     Classes definition
 */

/*
 *     Class CString
 */

int isl::CString::GetInteger(const std::string & sIn)
{
	try {
		return boost::lexical_cast<int>(sIn);
	}
	catch (...) {
		return 0;
	}
}

double isl::CString::GetDouble(const std::string & sIn)
{
	try {
		return boost::lexical_cast<double>(sIn);
	}
	catch (...) {
		return 0.0;
	}
}

unsigned int isl::CString::FromStrToInt(const std::string & sIn)
 {
	 unsigned int nNumber = 0;
	 size_t uMaxSize = sIn.size();
	 if (uMaxSize > 4) {
		 uMaxSize = 4;
	 }
	 for (size_t i = 0; i < uMaxSize; i++) {
		 nNumber += ((unsigned char)sIn[i]) << (8*i);
	 }
	 return nNumber;
 }

 std::string isl::CString::FromIntToStr(unsigned int uIn)
 {
	 std::string sId;
	 while (uIn != 0) {
		 sId.push_back((char )(uIn & 0xFF));
		 uIn = uIn >> 8;
	 }
	 return sId;
 }

 int isl::CString::Reset(char * sDest, size_t uSize)
{
	if (sDest == NULL) {
		return -1;
	}
	if (uSize == 0) {
		return -2;
	}
	for (std::size_t i = 0; i < uSize; i++) {
		sDest[i] = '\0';
	}
	return 0;
}

int isl::CString::Copy(char * sDest, size_t uSize, const char * sSrc)
{
#if defined(WIN32)
	return strcpy_s(sDest, uSize, sSrc);
#else // NOT WIN32
	strcpy(sDest, sSrc);
	return 0;
#endif // WIN32
}

int isl::CString::CopyN(char * sDest, size_t uSize, const char * sSrc, size_t uN)
{
#if defined(WIN32)
	return strncpy_s(sDest, uSize, sSrc, uN);
#else // NOT WIN32
	strncpy(sDest, sSrc, uN);
	return 0;
#endif // WIN32
}

int isl::CString::Concatenate(char * sDest, size_t uSize, const char * sSrc)
{
#if defined(WIN32)
	return strcat_s(sDest, uSize, sSrc);
#else // NOT WIN32
	strcat(sDest, sSrc);
	return 0;
#endif // WIN32
}

std::string isl::CString::ToWord(const std::string & sIn)
{
	if (sIn.empty()) {
		return std::string();
	}
	std::string sOut;
	for (std::string::size_type i = 0; i < sIn.size(); i++) {
		if ((sIn[i] >= 'a' && sIn[i] <= 'z') || (sIn[i] >= 'A' && sIn[i] <= 'Z')) {
			sOut += sIn[i];
		}
	}
	return sOut;
}

std::string isl::CString::Format(const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> vec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&vec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	return std::string(&vec[0]);
}

std::string isl::CString::Getsha1(const std::string & sIn)
{
	boost::uuids::detail::sha1 cSha1;
	cSha1.process_bytes(sIn.data(), sIn.size());
	unsigned uHash[5] = { 0 };
	cSha1.get_digest(uHash);
	char sBuf[41] = { 0 };
	for (int i = 0; i < 5; i++) {
		std::snprintf(sBuf + (i << 3), 41, "%08x", uHash[i]);
	}
	return std::string(sBuf);
}

int isl::CString::IsIdentifier(const std::string& sIn)
{
	for (std::string::size_type i = 0; i < sIn.size(); i++) {
		if (i == 0) {
			if ((sIn[i] >= 'a' && sIn[i] <= 'z') || (sIn[i] >= 'A' && sIn[i] <= 'Z') || sIn[i] == '_') {
				continue;
			}
			return -1;
		}
		if ((sIn[i] >= 'a' && sIn[i] <= 'z') || (sIn[i] >= 'A' && sIn[i] <= 'Z') || (sIn[i] >= '0' && sIn[i] <= '9') || sIn[i] == '_') {
			continue;
		}
		return -2;
	}
	return 0;
}

/*
 *     Class CApplication
 */

void isl::CApplication::SetLibraryDirectory(const std::string & sPath, bool bNative)
{
#ifdef WIN32
	if (bNative) {
		if (sPath.empty()) {
			SetDllDirectory(NULL);
		}
		else {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
			SetDllDirectory(conv.from_bytes(sPath).c_str());
		}
	}
	else {
		if (sPath.empty() == false) {
			boost::filesystem::current_path(boost::filesystem::path(sPath));
		}
	}
#else // Not WIN32
	if (sPath.empty() == false) {
		boost::filesystem::current_path(boost::filesystem::path(sPath));
	}
#endif
}

std::string isl::CApplication::GetRuntimePath()
{
	std::string sRes;
#ifdef WIN32
	wchar_t sPath[MAX_PATH];
	HMODULE hmLib = 0;
	if (GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
			GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
			(LPCWSTR)&isl::CApplication::GetRuntimePath, &hmLib) != 0) {
		if (GetModuleFileName(hmLib, sPath, sizeof(sPath)) != 0) {
			std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> conv;
			boost::filesystem::path bfPath(conv.to_bytes(sPath));
			sRes = bfPath.parent_path().string();
		}
	}
#else // Not WIN32
	Dl_info cInfo;
	if (dladdr(isl::CApplication::GetRuntimePath, &cInfo)) {
		boost::filesystem::path bfPath(cInfo.dli.fname);
		sRes = bfPath.parent_path().string();
	}
#endif
	return sRes;
}

int isl::CFileSystem::CreateEmptyFile(const std::string & sFileName)
{
#ifdef WIN32
	 return -1;
#else
	 int fd = ::open(sFileName.c_str(), O_EXCL | O_CREAT | O_RDWR, 0640);
	 if (-1 == fd) {
		 if (errno == EEXIST) {
			 return 0;
		 }
		 return -1;
	 }
	 else {
		 close(fd);
	 }
	 return 1;
#endif
}
