/*
 *     Name: isl_ini.h
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

		std::string ResolveParameter(const std::string & sVal);
		std::string GetParameter(const std::string & sPath, bool bResolve = false);
		void SetParameter(const std::string & sPath, const std::string & sValue);

	private:
		std::string m_sFile;
		boost::property_tree::ptree m_cTree;

		tStatus m_eStatus;
		std::string m_sParserError;
	};
}

#endif // _IPI_INI_H_
