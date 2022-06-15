/*
 *     Name: isl_log.h
 *
 *     Description: Log tools.
 *
 *     Modification date: 22/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
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