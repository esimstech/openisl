/*
 *     Name: tts_misc.cpp
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

/*
 *     Header files
 */

#ifdef WIN32
#include <windows.h>
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
 *     Class TString
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

/*
 *     Class TApplication
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
