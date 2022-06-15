/*
 *     Name: isl_ini.cpp
 *
 *     Description: INI tools.
 *
 *     Modification date: 28/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

 /*
  *     Header files
  */

#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/process/environment.hpp>

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

std::string isl::CINI::GetParameter(const std::string & sPath)
{
	return m_cTree.get<std::string>(sPath, "");
}

void isl::CINI::SetParameter(const std::string & sPath, const std::string & sValue)
{
	m_cTree.put(sPath, sValue);
}
