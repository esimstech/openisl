/*
 *     Name: isl_ini.cpp
 *
 *     Description: INI tools.
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

#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/process/environment.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>

#include "isl_ini.h"


/*
 *     Classes definition
 */

const std::string isl::CINI::c_sProgram = "openisl";

isl::CINI::CINI(const std::string & sFile, bool bUserPath)
{
	m_sFile = sFile;
	if (bUserPath) {
#ifdef WIN32
		m_sFile = boost::this_process::environment()["APPDATA"].to_string() + "\\" + c_sProgram + "\\" + sFile;
#else
		m_sFile = boost::this_process::environment()["HOME"].to_string() + "/.config/" + c_sProgram + "/" + sFile;
#endif
	}
	m_eStatus = SUCCESS;
}

isl::CINI::~CINI()
{
}

std::string isl::CINI::GetFile()
{
	return m_sFile;
}

isl::CINI::tStatus isl::CINI::GetStatus()
{
	return m_eStatus;
}

std::string isl::CINI::GetParserError()
{
	return m_sParserError;
}

bool isl::CINI::Load()
{
	if (m_sFile.empty()) {
		m_eStatus = NOFILE;
		return false;
	}
	if (boost::filesystem::exists(m_sFile) == false) {
		m_eStatus = FILENOTEXIST;
		return false;
	}
	try {
		boost::property_tree::read_ini(m_sFile, m_cTree);
	}
	catch (const boost::property_tree::ini_parser_error& e) {
		m_sParserError = e.filename() + ":" + std::to_string(e.line()) + ":" + e.message();
		m_eStatus = INIPARSERERROR;
		return false;
	}
	return true;
}

bool isl::CINI::Save(const std::string & sFile, bool bUserPath)
{
	std::string sFileToSave = m_sFile;
	if (sFile.empty() == false) {
		sFileToSave = sFile;
		if (bUserPath) {
#ifdef WIN32
			sFileToSave = boost::this_process::environment()["APPDATA"].to_string() + "\\" + c_sProgram + "\\" + sFile;
#else
			sFileToSave = boost::this_process::environment()["HOME"].to_string() + "/.config/" + c_sProgram + "/" + sFile;
#endif
		}
	}
	try {
		boost::property_tree::write_ini(sFileToSave, m_cTree);
	}
	catch (const boost::property_tree::ini_parser_error& e) {
		m_sParserError = e.filename() + ":" + std::to_string(e.line()) + ":" + e.message();
		m_eStatus = INIPARSERERROR;
		return false;
	}
	return true;
}

std::string isl::CINI::ResolveParameter(const std::string & sVal)
{
	const boost::regex cRe("\\$\\(([^\\)]+)\\)");
	std::string sRes(sVal);
	for (boost::sregex_iterator i = boost::sregex_iterator(sVal.begin(),
			sVal.end(), cRe); i != boost::sregex_iterator(); ++i) {
		boost::smatch cWhat = *i;
		boost::replace_first(sRes, cWhat.str(),
			boost::this_process::environment()[cWhat[1].str().c_str()].to_string());
	}
	return sRes;
}

std::string isl::CINI::GetParameter(const std::string & sPath, bool bResolve)
{
	std::string sVal = m_cTree.get<std::string>(sPath, "");
	if (bResolve) {
		sVal = ResolveParameter(sVal);
	}
	return sVal;
}

void isl::CINI::SetParameter(const std::string & sPath, const std::string & sValue)
{
	m_cTree.put(sPath, sValue);
}
