/*
 *     Name: fmusim.cpp
 *
 *     Description: FMI Master Simulator class.
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

#include <isl_log.h>

#include "fmusim_const.h"
#include "fmusim.h"


/*
 *     Classes definition
 */

CFMUSim::CFMUSim(CModel * cModel)
{
	m_cModel = cModel;
	m_cRunThread = 0;
	m_dSimulationRate = 0.0;
}

CFMUSim::~CFMUSim()
{
	Stop();
	if (m_cModel != 0) {
		delete m_cModel;
	}
	m_cModel = 0;
	//
	if (m_cRunThread) {
		delete m_cRunThread;
	}
	m_cRunThread = 0;
}

bool CFMUSim::Run()
{
	int nErrorCode = 0;
	if (IsReadyToRun(nErrorCode) == false) {
		AppLogError(ERROR_FMUSIM_RUNFAILED,
			"Simulation run failed (%d). Please load a valid model first.", nErrorCode);
		return false;
	}
	if (m_cRunThread != 0) {
		if (m_cRunThread->Stopped()) {
			delete m_cRunThread;
			m_cRunThread = 0;
		}
		else {
			AppLogWarning(WARNING_FMUSIM_SIMRUNNING,
				"A simulation is already running.");
			return false;
		}
	}
	m_cRunThread = new CRunThread(this);
	m_cRunThread->Start();
	return true;
}

bool CFMUSim::Stop()
{
	if (m_cRunThread == 0) {
		return true; // Already stopped
	}
	m_cRunThread->Stop();
	delete m_cRunThread;
	m_cRunThread = 0;
	return true;
}

bool CFMUSim::Wait()
{
	if (m_cRunThread == 0) {
		return true; // Already stopped
	}
	m_cRunThread->Join();
	return true;
}

bool CFMUSim::IsRunning()
{
	if (m_cRunThread == NULL) {
		return false;
	}
	return m_cRunThread->GetStatus() == isl::CThread::THREAD_STOPPED;
}

bool CFMUSim::IsReadyToRun(int & nErrorCode)
{
	if (m_cModel == NULL) {
		return false;
	}
	return m_cModel->Validate(nErrorCode);
}

void CFMUSim::SetSimulationRate(double dVal)
{
	if ((dVal > 0.0) && (m_cModel != 0)) {
		m_cModel->SetSimSpeed(dVal);
		m_dSimulationRate = dVal;
		int nVal = (int)m_dSimulationRate;
	}
}

CModel * CFMUSim::GetModel()
{
	return m_cModel;
}
