/*
 *     Name: isl_log.cpp
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

/*
 *     Header files
 */

#include <ostream>

#include "isl_log.h"
#include "isl_misc.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/process/environment.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>


/*
 *     Macros and constants definition
 */

#ifdef WIN32
#define DEFAULT_LOG_FILE boost::str(boost::format("%1%\\isl_app.log") % boost::filesystem::temp_directory_path().string())
#else
#define DEFAULT_LOG_FILE boost::str(boost::format("%1%/isl_app.log") % boost::filesystem::temp_directory_path().string())
#endif
#define ISL_DEBUG_LEVEL "ISL_DEBUG_LEVEL"


/*
 *     Classes definition
 */

isl::CMsgLogger::CMsgLogger(const char * sFile, int nLine)
{
	m_sFile = sFile;
	m_nLine = nLine;

	m_sContext = boost::str(boost::format("%1%:%2%") % sFile % nLine);
	try {
		m_uDebugLevel = boost::lexical_cast<int>(boost::this_process::environment()["ISL_DEBUG_LEVEL"].to_string());
	}
	catch (...) {
		m_uDebugLevel = 0;
	}
}

void isl::CMsgLogger::Info(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	std::string sMsg = boost::str(boost::format("[%1%]: %2%") % uId % &sVec[0]);
	isl::CLogHandler::MessageHandler(isl::CLogHandler::MSG_INFO, m_sContext, sMsg);
}

void isl::CMsgLogger::Warning(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	std::string sMsg = boost::str(boost::format("[%1%]: %2%") % uId % &sVec[0]);
	isl::CLogHandler::MessageHandler(isl::CLogHandler::MSG_WARNING, m_sContext, sMsg);
}

void isl::CMsgLogger::Error(unsigned int uId, const char * sFormat, ...)
{
	va_list lArgs;
	va_start(lArgs, sFormat);
	size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, sFormat);
	std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
	va_end(lArgs);
	std::string sMsg = boost::str(boost::format("[%1%]: %2%") % uId % &sVec[0]);
	isl::CLogHandler::MessageHandler(isl::CLogHandler::MSG_ERROR, m_sContext, sMsg);
}

void isl::CMsgLogger::Debug(unsigned int uLevel, unsigned int uId, const char * sFormat, ...)
{
	if (m_uDebugLevel >= uLevel) {
		va_list lArgs;
		va_start(lArgs, sFormat);
		size_t len = std::vsnprintf(NULL, 0, sFormat, lArgs);
		va_end(lArgs);
		std::vector<char> sVec(len + 1);
		va_start(lArgs, sFormat);
		std::vsnprintf(&sVec[0], len + 1, sFormat, lArgs);
		va_end(lArgs);
		std::string sMsg = boost::str(boost::format("[L%1%] [%2%]: %3%") % uLevel % uId % &sVec[0]);
		isl::CLogHandler::MessageHandler(isl::CLogHandler::MSG_DEBUG, m_sContext, sMsg);
	}
}

isl::CLogHandler * isl::CLogHandler::m_cInstance = NULL;

isl::CLogHandler::CLogHandler()
{
	m_sLogFile = DEFAULT_LOG_FILE;
	m_ofLog = NULL;
	m_bUseHeader = false;
	m_bUseContext = false;
}

isl::CLogHandler::~CLogHandler()
{
	if (m_ofLog != NULL) {
		m_ofLog->close();
		delete m_ofLog;
	}
	m_ofLog = NULL;
}

void isl::CLogHandler::Init(const std::string & sFile)
{
	m_sLogFile = sFile;
	if (m_sLogFile.empty() == false) {
		// Close old
		if (m_ofLog != NULL) {
			m_ofLog->close();
			delete m_ofLog;
			m_ofLog = NULL;
		}
		// Create new
		m_ofLog = new std::ofstream(m_sLogFile);
		if (m_ofLog->bad()) {
			delete m_ofLog;
			m_ofLog = NULL;
			// using the default log file instead
			m_ofLog = new std::ofstream(DEFAULT_LOG_FILE);
			if (m_ofLog->bad()) {
				delete m_ofLog;
				m_ofLog = NULL;
			}
		}
	}
}

void isl::CLogHandler::SetHeader(const std::string & sHeader)
{
	m_bUseHeader = sHeader.empty() == false;
	m_sHeader = sHeader;
}

void isl::CLogHandler::MessageHandler(tMsgType eType, const std::string & sContext,
	const std::string & sMessage)
{
	static boost::mutex cSync;
	cSync.lock();
	//
	std::string sToPrint;
	std::ostream * fOut = &std::cout;
	switch (eType) {
		case isl::CLogHandler::MSG_WARNING:
			fOut = &std::cerr;
			sToPrint = "Warning";
			break;
		case isl::CLogHandler::MSG_ERROR:
			fOut = &std::cerr;
			sToPrint = "Error";
			break;
		case isl::CLogHandler::MSG_DEBUG:
			fOut = &std::cout;
			sToPrint = "Debug";
			break;
		case isl::CLogHandler::MSG_INFO:
		default:
			fOut = &std::cout;
			sToPrint = "Info";
			break;
	}
	static std::locale loc(std::cout.getloc(), new boost::posix_time::time_facet("%Y.%m.%d %H:%M:%S"));
	std::stringstream sStream;
	sStream.imbue(loc);
	sStream << boost::posix_time::second_clock::local_time();
	sToPrint.append(boost::str(boost::format(" (%1%)") % sStream.str()));
	if (AppLog->IsHeaderUsed()) {
		sToPrint.append(boost::str(boost::format(" (%1%)") % AppLog->GetHeader()));
	}
	sToPrint.append(boost::str(boost::format(" : %1%") % sMessage));
	if (AppLog->IsContextUsed()) {
		sToPrint.append(boost::str(boost::format(" (%1%)") % sContext));
	}
	// Print to stderr/stdout
	*fOut << sToPrint << std::endl;

	// Print in log file
	std::ofstream * ofLog = AppLog->GetLog();
	if (ofLog != NULL) {
		if (ofLog->bad() == false) {
			*ofLog << sToPrint << std::endl;
		}
	}
	cSync.unlock();
}

isl::CLogHandler * isl::CLogHandler::Instance()
{
	if (m_cInstance == NULL) {
		m_cInstance = new isl::CLogHandler();
	}
	return m_cInstance;
}

void isl::CLogHandler::Close()
{
	if (m_cInstance != NULL) {
		delete m_cInstance;
		m_cInstance = NULL;
	}
}
