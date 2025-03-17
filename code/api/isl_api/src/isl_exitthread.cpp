/*
 *     Name: isl_exitthread.cpp
 *
 *     Description: Thread used to exit an isl session.
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

#include <boost/format.hpp>

#include <isl_log.h>
#include "isl_errorcodes.h"
#include "isl_api.h"
#include "isl_settings.h"
#include "isl_exitthread.h"


/*
 *     Constants definition
 */

#define SEM_KEY_KILLER			"_isl_sem_xxx%1%_"
#define SEM_SESSION_KEY_KILLER	"_isl_sem_xse%1%_"

const std::string isl::CExitThread::c_sKeyGeneralId = "I3RP4eU1c6Ckndf2";


/*
 *     Classes definition
 */

bool isl::CExitThread::SendStopRequest(const std::string & sSession)
{
	std::string sKey = boost::str(boost::format(SEM_KEY_KILLER) % c_sKeyGeneralId);
	if (sSession.empty() == false) {
		sKey = boost::str(boost::format(SEM_SESSION_KEY_KILLER) % sSession);
	}
	CSem cSem(sKey, 0, CSem::OPEN);
	if (cSem.Release(1) == false) {
		return false;
	}
	return true;
}

isl::CExitThread::CExitThread(CConnect * cConnect, bool bMySession) : isl::CThread::CThread()
{
	std::string sKey = boost::str(boost::format(SEM_KEY_KILLER) % c_sKeyGeneralId);
	if ((bMySession == true) && (cConnect != NULL)) {
		sKey = boost::str(boost::format(SEM_SESSION_KEY_KILLER) % cConnect->GetSessionId());
	}
	bool bIsGlobalIPC = CAppSettings().IsGlobalIPC();
	m_cSem = 0;
	if (CAppSettings().IsISLCompatible()) {
		m_cSem = new CSem(sKey, "qipc_systemsem_", 0, CSem::CREATE, bIsGlobalIPC);
	}
	else {
		m_cSem = new CSem(sKey, 0, CSem::CREATE, bIsGlobalIPC);
	}
	m_cConnect = cConnect;
}

isl::CExitThread::~CExitThread()
{
	delete m_cSem;
	m_cConnect = NULL;
}

void isl::CExitThread::Run()
{
	// Wait on the semaphore
	if (m_cSem->Acquire() == false) {
		return;
	}
	AppLogWarning(ISLEXITTHREAD_STOP_RECEIVED, "A stop request have been received.");
	bool bQuit = true;
	if (m_cConnect != NULL) {
		if (m_cConnect->GetMode() == CConnect::SMD_STOP) {
			bQuit = false;
		}
	}
	if (bQuit) {
		AppLogWarning(ISLEXITTHREAD_EXITINGAPP, "Exiting the application.");
		exit(0);
	}
	else {
		AppLogInfo(ISLEXITTHREAD_SETTERMINATED, "ISL session status set to 'terminated'");
		m_cConnect->SetTerminated();
	}
}
