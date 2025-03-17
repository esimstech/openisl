/*
 *     Name: isl_log.h
 *
 *     Description: Log tools.
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

#ifndef _ISL_LOG_H_
#define _ISL_LOG_H_

/*
 *     Header files
 */

#include <string>
#include <fstream>


/*
 *     Macros definition
 */

#define LOG_FILE_EXT	"log"


/*
 *     Classes declaration
 */

namespace isl {
	class CMsgLogger {
	public:
		CMsgLogger(const char * sFile, int nLine);
		~CMsgLogger() {}

		void Info(unsigned int uId, const char * sFormat, ...);
		void Warning(unsigned int uId, const char * sFormat, ...);
		void Error(unsigned int uId, const char * sFormat, ...);

		void Debug(unsigned int uLevel, unsigned int uId, const char * sFormat, ...);

	private:
		std::string m_sFile;
		int m_nLine;

		std::string m_sContext;
		unsigned int m_uDebugLevel;
	};

	class CLogHandler {
	public:
		enum tMsgType {
			MSG_DEBUG = 0,
			MSG_INFO,
			MSG_WARNING,
			MSG_ERROR
		};

		CLogHandler();
		~CLogHandler();

		void Init(const std::string & sFile);

		std::string GetLogFile() { return m_sLogFile; }
		std::ofstream * GetLog() { return m_ofLog; }

		void UseContext(bool bVal) { m_bUseContext = bVal; }
		bool IsContextUsed() { return m_bUseContext; }

		void SetHeader(const std::string & sHeader);
		std::string GetHeader() { return m_sHeader; }
		bool IsHeaderUsed() { return m_bUseHeader; }

		static void MessageHandler(tMsgType eType, const std::string & sContext,
			const std::string & sMessage);

	private:
		std::string m_sLogFile;
		std::string m_sHeader;
		bool m_bUseHeader;
		bool m_bUseContext;

		std::ofstream * m_ofLog;

		static CLogHandler * m_cInstance;

	public:
		static CLogHandler * Instance();
		static void Close();
	};

}

#define AppLog			isl::CLogHandler::Instance()
#define AppLog_Close	isl::CLogHandler::Close()

#define AppLogInfo		isl::CMsgLogger(__FILE__, __LINE__).Info
#define AppLogWarning	isl::CMsgLogger(__FILE__, __LINE__).Warning
#define AppLogError		isl::CMsgLogger(__FILE__, __LINE__).Error
#define AppLogDebug		isl::CMsgLogger(__FILE__, __LINE__).Debug


#endif // _ISL_LOG_H_