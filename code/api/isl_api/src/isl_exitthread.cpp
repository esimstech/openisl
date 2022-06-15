/*
 *     Name: isl_exitthread.cpp
 *
 *     Description: Thread used to exit an isl session.
 *
 *     Modification date: 22/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

/*
 *     Header files
 */

#include <boost/format.hpp>

#include <isl_log.h>
#include "isl_errorcodes.h"
#include "isl_api.h"
#include "appsettings.h"
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
	m_cSem = 0;
	if (CAppSettings().IsISLCompatible()) {
		m_cSem = new CSem(sKey, "qipc_systemsem_", 0, CSem::CREATE);
	}
	else {
		m_cSem = new CSem(sKey, 0, CSem::CREATE);
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
