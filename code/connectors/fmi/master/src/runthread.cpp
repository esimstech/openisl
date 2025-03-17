/*
 *     Name: runthread.cpp
 *
 *     Description: RunThread class.
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

#include <boost/filesystem.hpp>
#include <isl_log.h>

#include "fmusim.h"
#include "runthread.h"

/*
 *     Classes definition
 */

CRunThread::CRunThread(CFMUSim * cSim) : isl::CThread()
{
	m_cSim = cSim;
	m_eState = RUN_STATE_ENTRY;
	m_bStop = false;
	m_dTime = 0.0;
	m_dStepSize = 0.0;
	m_dStepTolerance = 0.0;
	m_dStopTime = 0.0;
	m_bUseTimer = false;
	m_dStepProgress = 0.0;
	m_dProgress = 0.0;
}

CRunThread::~CRunThread()
{
}

void CRunThread::Stop()
{
	m_bStop = true;
}

bool CRunThread::Stopped()
{
	return (m_bStop && (GetStatus() == isl::CThread::THREAD_STOPPED));
}

void CRunThread::Run()
{
	if (m_cSim == NULL) {
		AppLogError(ERROR_RUNTH_NOINSTANCE, "No instance of the FMU simulator found.");
		QuitApp();
		return;
	}
	int nErrorCode = 0;
	if (m_cSim->IsReadyToRun(nErrorCode) == false) {
		AppLogError(ERROR_RUNTH_NOTREADY, "The model is not ready to run. Error code: %d.", nErrorCode);
		QuitApp();
		return;
	}
	// Set the working directory to the model directory
	boost::filesystem::path bfpCurrentDir =
		boost::filesystem::path(boost::filesystem::absolute(m_cSim->GetModel()->GetFile())).parent_path();
	if (boost::filesystem::current_path() != bfpCurrentDir) {
		AppLogInfo(INFO_RUNTH_CHDIR, "Changing the current working directory from '%s' to '%s'",
			boost::filesystem::current_path().string().c_str(), bfpCurrentDir.string().c_str());
		boost::filesystem::current_path(bfpCurrentDir);
	}
	// Initialize the times
	isl::CConnect * cBlackBox = m_cSim->GetModel()->GetBlackBox();
	m_dTime = cBlackBox->GetStartTime();
	if (m_dTime < 0.0) {
		AppLogWarning(WARNING_RUNTH_STARTTIME, "Wrong start time: %gs. Value set to 0.0s.", m_dTime);
		m_dTime = 0.0;
	}
	m_dStopTime = cBlackBox->GetEndTime();
	if (m_dStopTime <= m_dTime) {
		AppLogError(ERROR_RUNTH_STOPTIME, "Wrong end time: %gs.", m_dStopTime);
		QuitApp();
		return;
	}
	m_dStepSize = cBlackBox->GetStepSize();
	if (m_dStepSize <= 0.0) {
		AppLogError(ERROR_RUNTH_STEPSIZE, "Wrong step size: %gs.", m_dStepSize);
		QuitApp();
		return;
	}
	m_dStepProgress = m_dStepSize * 100.0 / m_dStopTime;
	m_dStepTolerance = cBlackBox->GetStepTolerance();
	if (m_dStepTolerance <= 0.0) {
		AppLogError(ERROR_RUNTH_STEPTOLERANCE, "Wrong step tolerance: %g.", m_dStepTolerance);
		QuitApp();
		return;
	}
	//
	m_eState = RUN_STATE_ENTRY;
	// Initialization
	if (InitISL() == false) {
		QuitApp();
		return;
	}
	if (InitFMU() == false) {
		if (CloseISL() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEISL, "An error occurred when closing the ISL API.");
		}
		QuitApp();
		return;
	}
	if (InitVariables() == false) {
		if (CloseFMU() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEFMU, "An error occurred when closing the FMU simulation.");
		}
		if (CloseISL() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEISL,"An error occurred when closing the ISL API.");
		}
		QuitApp();
		return;
	}
	if (InitEnterFMU() == false) {
		if (CloseFMU() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEFMU, "An error occurred when closing the FMU simulation.");
		}
		if (CloseISL() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEISL, "An error occurred when closing the ISL API.");
		}
		QuitApp();
		return;
	}
	if (InitVariables() == false) {
		if (CloseFMU() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEFMU, "An error occurred when closing the FMU simulation.");
		}
		if (CloseISL() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEISL, "An error occurred when closing the ISL API.");
		}
		QuitApp();
		return;
	}
	if (InitExitFMU() == false) {
		if (CloseFMU() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEFMU, "An error occurred when closing the FMU simulation.");
		}
		if (CloseISL() == false) {
			AppLogWarning(WARNING_RUNTH_CLOSEISL, "An error occurred when closing the ISL API.");
		}
		QuitApp();
		return;
	}
	// Main loop
	m_eState = RUN_STATE_RUNNING;
	if (m_cSim->GetModel()->IsSimSpeedValid()) {
		if (m_dStepSize >= 1e-3) {
			m_bUseTimer = true;
			m_ctSimTimeRef = boost::chrono::high_resolution_clock::now();
		}
	}
	AppLogInfo(INFO_RUNTH_SIMSTARTED, "Simulation is started...");
	while (m_bStop == false) {
		// Compute a step
		if (ComputeStep() == false) {
			m_bStop = true;
			break;
		}
		// Send outputs to the co-simulation bus
		if (GetOutputs() == false) {
			m_bStop = true;
			break;
		}
		// Set inputs to the simulator
		if (SetInputs() == false) {
			m_bStop = true;
			break;
		}
		m_bStop = cBlackBox->IsTerminated();
	}
	AppLogInfo(INFO_RUNTH_SIMSTOPPED, "Simulation is stopped...");
	// Closing
	if (CloseFMU() == false) {
		AppLogWarning(WARNING_RUNTH_CLOSEFMU, "An error occurred when closing the FMU simulation.");
	}
	if (CloseISL() == false) {
		AppLogWarning(WARNING_RUNTH_CLOSEISL, "An error occurred when closing the ISL API.");
	}
	// Quit if the option set
	QuitApp();
}

bool CRunThread::InitISL()
{
	CModel * cModel = m_cSim->GetModel();
	isl::CConnect * cISLModel = cModel->GetBlackBox();
	std::string sSession = cModel->GetSession();
	cISLModel->SetSessionId(sSession);
	if (cISLModel->Create() == false) {
		AppLogError(ERROR_RUNTH_CREATESESSION,
			"Failed to create the session '%s'.", sSession.c_str());
		return false;
	}
	bool bOk = true;
	if (cModel->m_lOutputs.empty() == false) {
		CModelVars::iterator iVar;
		for (iVar = cModel->m_lOutputs.begin(); iVar != cModel->m_lOutputs.end(); ++iVar) {
			if ((*iVar)->InitializeISL(m_dTime) == false) {
				bOk = false;
			}
		}
	}
	if (bOk == false) {
		AppLogError(ERROR_RUNTH_INITVARS, "Failed to initialize the ISL variables.");
		return false;
	}
	if (cISLModel->ListenToExitSession() == false) {
		AppLogWarning(WARNING_RUNTH_LISTENEXITSESSION, "ListenToExitSession failed.");
	}
	AppLogInfo(INFO_RUNTH_ISLCONNECT, "ISL API: Waiting for the transmitters connection...");
	if (cISLModel->Connect(true) == false) {
		AppLogError(ERROR_RUNTH_ISLCONNECT, "Failed to connect the inputs");
		return false;
	}
#if 0 // TODO: To implement
	if (m_cSim->GetModel()->GetStore()) {
		if (cISLModel->OpenStore() == false) {
			AppLogWarning(TBD, "Failed to open data storage.");
		}
		else {
			AppLogInfo(TBD, "ISL API: Ready to store data.");
		}
	}
#endif
	AppLogInfo(INFO_RUNTH_INITDONE, "ISL API: Initialization done.");
	m_eState = RUN_STATE_ISL_INITIALIZED;
	return true;
}

bool CRunThread::InitFMU()
{
	CGenericSlave * cSlave = m_cSim->GetModel()->GetSlave();
	// Load the library
	if (cSlave->MapFunctions() == false) {
		AppLogError(ERROR_RUNTH_MAPFUNCTIONS,
			"Failed to load the model executable.", cSlave->GetLibraryName().c_str());
		AppLogError(ERROR_RUNTH_MF_ERRORMSG, "Error %d: %s",
			cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
		if (cSlave->FreeLibrary() == false) {
			AppLogWarning(WARNING_RUNTH_FAILED_FREELIB, "Failed to free the library.");
		}
		return false;
	}
	// Instantiate the slave model
	if (cSlave->Instantiate() == false) {
		AppLogError(ERROR_RUNTH_INSTANTIATE, "Failed to instantiate the slave model.");
		AppLogError(ERROR_RUNTH_INST_ERRORMSG, "Error %d: %s",
			cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
		if (cSlave->FreeLibrary() == false) {
			AppLogWarning(WARNING_RUNTH_FAILED_FREELIB, "Failed to free the library.");
		}
		return false;
	}
	AppLogInfo(INFO_RUNTH_INSTANTIATED, "FMU simulation: model instantiated.");
	m_eState = RUN_STATE_FMI_INSTANTIATED;
	return true;
}

bool CRunThread::InitEnterFMU()
{
	CGenericSlave * cSlave = m_cSim->GetModel()->GetSlave();
	// Start the initialization of the slave model
	if (cSlave->Initialize() == false) {
		AppLogError(ERROR_RUNTH_INITIALIZE,
			"Failed to start the initialization of the FMU simulation.");
		AppLogError(ERROR_RUNTH_INIT_ERRORMSG, "Error %d: %s",
			cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
		return false;
	}
	AppLogInfo(INFO_RUNTH_FMUINITDONE, "FMU simulation: Entering in initialization mode.");
	m_eState = RUN_STATE_FMI_ENTER_INIT;
	return true;
}

bool CRunThread::InitExitFMU()
{
	CGenericSlave * cSlave = m_cSim->GetModel()->GetSlave();
	// End the initialization of the slave model
	if (cSlave->EndInitialize() == false) {
		AppLogError(ERROR_RUNTH_ENDINITIALIZE,
			"Failed to end the initialization of the FMU simulation.");
		AppLogError(ERROR_RUNTH_ENDINIT_ERRORMSG, "Error %d: %s",
			cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
		return false;
	}
	AppLogInfo(INFO_RUNTH_FMUENDINITDONE, "FMU simulation: Exiting the initialization mode.");
	m_eState = RUN_STATE_FMI_ENTER_INIT;
	return true;
}

bool CRunThread::InitVariables()
{
	CModel * cModel = m_cSim->GetModel();
	CModelVars * lVars = NULL;
	switch (m_eState) {
		case RUN_STATE_FMI_INSTANTIATED:
			lVars = &(cModel->m_lInitVars1rst);
			break;
		case RUN_STATE_FMI_ENTER_INIT:
			lVars = &(cModel->m_lInitVars2nd);
			break;
		default:
			break;
	}
	bool bOk = true;
	if (lVars != NULL) {
		if (lVars->empty() == false) {
			CModelVars::iterator iVar;
			for (iVar = lVars->begin(); iVar != lVars->end(); ++iVar) {
				if ((*iVar)->InitializeModel() == false) {
					bOk = false;
				}
			}
		}
	}
	if (bOk == false) {
		AppLogError(ERROR_RUNTH_FMUINITVARS, "Failed to initialize the FMU variables.");
	}
	return bOk;
}

bool CRunThread::ComputeStep()
{
	if (m_dTime + (m_dStepSize * m_dStepTolerance) >= m_dStopTime) {
		AppLogInfo(INFO_RUNTH_SIMCOMPLETED, "The simulation has completed.");
		return false;
	}
	CGenericSlave * cSlave = m_cSim->GetModel()->GetSlave();
	// Do the step - TODO : Implements set states
	if (cSlave->DoStep(m_dTime, m_dStepSize, true) == false) {
		AppLogError(ERROR_RUNTH_DOSTEP, "Failed to compute time %gs.", m_dTime);
		AppLogError(ERROR_RUNTH_DOSTEP_ERRORMSG, "Error %d: %s",
			cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
		return false;
	}
	m_dTime += m_dStepSize;
	// Simulation speed constraint
	if (m_bUseTimer) {
		double dStepTime = (double)boost::chrono::duration_cast<boost::chrono::milliseconds>(
			boost::chrono::high_resolution_clock::now() - m_ctSimTimeRef).count();
		if (dStepTime < 0.0) {
			dStepTime = 0.0;
		}
		double dStepSize = m_dStepSize * 1000;
		if (dStepTime < dStepSize / m_cSim->GetModel()->GetSimSpeed()) {
			double dWaitTime = (dStepSize - dStepTime) / m_cSim->GetModel()->GetSimSpeed();
			isl::CThread::Sleep((unsigned long)dWaitTime);
		}
		m_ctSimTimeRef = boost::chrono::high_resolution_clock::now();
	}
	m_dProgress += m_dStepProgress;
	//m_cSim->SetProgress(m_dProgress);
	//
	return true;
}

bool CRunThread::SetInputs()
{
	bool bRet = true;
	CModel * cModel = m_cSim->GetModel();
	if (cModel->m_lInputs.empty() == false) {
		CModelVars::iterator iVar;
		for (iVar = cModel->m_lInputs.begin(); iVar != cModel->m_lInputs.end(); ++iVar) {
			if ((*iVar)->TransferDataISLToModel(m_dTime) == false) {
				AppLogError(ERROR_RUNTH_VARISLTOFMU,
					"Variable %s: issue on transferring data from ISL to the FMU.",
					(*iVar)->GetIO()->GetId().c_str());
				bRet = false;
			}
		}
	}
	return bRet;
}

bool CRunThread::GetOutputs()
{
	bool bRet = true;
	CModel * cModel = m_cSim->GetModel();
	if (cModel->m_lOutputs.empty() == false) {
		CModelVars::iterator iVar;
		for (iVar = cModel->m_lOutputs.begin(); iVar != cModel->m_lOutputs.end(); ++iVar) {
			if ((*iVar)->TransferDataModelToISL(m_dTime) == false) {
				AppLogError(ERROR_RUNTH_VARFMUTOISL,
					"Variable %s: issue on transferring data from the FMU to ISL.",
					(*iVar)->GetIO()->GetId().c_str());
				bRet = false;
			}
		}
	}
	return bRet;
}

bool CRunThread::CloseISL()
{
	bool bOk = true;
	isl::CConnect * cISLModel = m_cSim->GetModel()->GetBlackBox();
#if 0 // TODO: To implement
	if (m_cSim->GetModel()->GetStore()) {
		cISLModel->CloseStore();
		AppLogInfo(TBD, "ISL API: Data storage closed.");
	}
#endif
	if (cISLModel->Disconnect() == false) {
		AppLogError(ERROR_RUNTH_ISLDISCONNECT, "Failed to disconnect from the ISL API.");
		bOk = false;
	}
	if (cISLModel->SendStopSession() == false) {
		AppLogError(ERROR_RUNTH_SENDSTOPSESSION,
			"Failed to send a stop request to the connected applications.");
		bOk = false;
	}
	AppLogInfo(INFO_RUNTH_CLOSEISL, "ISL API: closing the simulation.");
	m_eState = RUN_STATE_ISL_CLOSED;
	return bOk;
}

bool CRunThread::CloseFMU()
{
	bool bOk = true;
	CGenericSlave * cSlave = m_cSim->GetModel()->GetSlave();
	if (m_eState == RUN_STATE_FMI_EXIT_INIT) {
		if (cSlave->Terminate() == false) {
			AppLogError(ERROR_RUNTH_FMUTERMINATE, "Failed to terminate the FMU simulation.");
			AppLogError(ERROR_RUNTH_TERMINATE_ERRORMSG, "Error %d: %s",
				cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
			bOk = false;
		}
	}
	if ((m_eState == RUN_STATE_FMI_INSTANTIATED) ||
		(m_eState == RUN_STATE_FMI_ENTER_INIT) ||
		(m_eState == RUN_STATE_FMI_EXIT_INIT)) {
		if (cSlave->Free() == false) {
			AppLogError(ERROR_RUNTH_FMUFREE, "Failed to free the FMU instance.");
			AppLogError(ERROR_RUNTH_FREEFMU_ERRORMSG, "Error %d: %s",
				cSlave->GetErrorCode(), cSlave->GetErrorString().c_str());
			bOk =  false;
		}
	}
	if (cSlave->FreeLibrary() == false) {
		AppLogError(ERROR_RUNTH_FREELIB, "Failed to free the library.");
		bOk = false;
	}
	AppLogInfo(INFO_RUNTH_CLOSINGFMU, "FMU simulation: closing the simulation.");
	m_eState = RUN_STATE_FMI_CLOSED;
	return bOk;
}

void CRunThread::QuitApp()
{
	exit(100);
}