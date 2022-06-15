/*
 *     Name: isl_ini.h
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

#ifndef _ISL_INI_H_
#define _ISL_INI_H_

/*
 *     Header files
 */

#include <vector>
#include <string>
#include <map>

#include <boost/property_tree/ptree.hpp>


/*
 *     Classes declaration
 */

namespace isl {
	class CINI
	{
	public:
		enum tStatus {
			SUCCESS = 0,
			NOFILE,
			FILENOTEXIST,
			INIPARSERERROR,
			UNKNOWN
		};

		static const std::string c_sProgram;

		CINI(const std::string & sFile, bool bUserPath = false);
		~CINI();

		std::string GetFile();
		tStatus GetStatus();
		std::string GetParserError();

		bool Load();
		bool Save(const std::string & sFile = "", bool bUserPath = false);

		std::string GetParameter(const std::string & sPath);
		void SetParameter(const std::string & sPath, const std::string & sValue);

	private:
		std::string m_sFile;
		boost::property_tree::ptree m_cTree;

		tStatus m_eStatus;
		std::string m_sParserError;
	};
}

#endif // _IPI_INI_H_
