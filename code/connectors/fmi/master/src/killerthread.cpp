/*
 *     Name: killerthread.cpp
 *
 *     Description: Thread class used to send and receive stop signals.
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

#include <cstdlib>
#include <isl_log.h>

#include "fmusim_const.h"
#include "fmusim.h"
#include "killerthread.h"


/*
 *     Classes definition
 */

const bool CKillerThread::SendStopRequest()
{
	isl::CSem cSem(SEM_KEY_KILLER, 0, isl::CSem::OPEN);
	if (cSem.Release(1) == false) {
		AppLogError(ERROR_KILLTHREAD_SENDSTOP,
			"Failed to send a stop request. Error %d", cSem.GetError());
		return false;
	}
	return true;
}

CKillerThread::CKillerThread(CFMUSim * cSim) : isl::CThread()
{
	m_cSem = new isl::CSem(SEM_KEY_KILLER, 0, isl::CSem::CREATE);
	m_cSim = cSim;
}

CKillerThread::~CKillerThread()
{
	try {
		delete m_cSem;
	}
	catch (...) {
		AppLogWarning(WARNING_KILLTHREAD_SEMDELETE,
			"Failed to delete the system semaphore associated to the 'stop simulation' thread.");
	}
}

void CKillerThread::Run()
{
	// Wait on the semaphore
	if (m_cSem->Acquire() == false) {
		AppLogError(ERROR_KILLTHREAD_SEMACQUIRE,
			"Failed to acquire a system semaphore ('stop simulation' thread). Error %d",
			m_cSem->GetError());
		return;
	}
	AppLogWarning(WARNING_KILLTHREAD_STOPRECEIVED,
		"A stop request have been received. Exiting the application.");
	if (m_cSim != NULL) {
		if (m_cSim->IsRunning()) {
			AppLogWarning(WARNING_KILLTHREAD_SIMRUNNING,
				"A simulation is still running. Trying to stop it...");
			if (m_cSim->Stop() == false) {
				AppLogError(ERROR_KILLTHREAD_SIMSTOPFAILED,
					"Failed to stop the current simulation.");
			}
		}
	}
	std::exit(-5);
}
