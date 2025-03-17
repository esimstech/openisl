/*
 *     Name: runthread.h
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

#ifndef _RUNTHREAD_H_
#define _RUNTHREAD_H_

/*
 *     Header files
 */

#include <boost/chrono.hpp>
#include <isl_thread.h>


/*
 *     Classes declaration
 */

class CFMUSim;
class CRunThread : public isl::CThread
{
public:
	enum tState {
		RUN_STATE_ENTRY,
		RUN_STATE_ISL_INITIALIZED,
		RUN_STATE_FMI_INSTANTIATED,
		RUN_STATE_FMI_ENTER_INIT,
		RUN_STATE_FMI_EXIT_INIT,
		RUN_STATE_RUNNING,
		RUN_STATE_FMI_CLOSED,
		RUN_STATE_ISL_CLOSED
	};

	CRunThread(CFMUSim * sSim);
	~CRunThread();

	void Stop();
	bool Stopped();

protected:
	void Run();

	bool InitISL();
	bool InitFMU();
	bool InitEnterFMU();
	bool InitExitFMU();
	bool InitVariables();

	bool ComputeStep();
	bool SetInputs();
	bool GetOutputs();

	bool CloseISL();
	bool CloseFMU();

	void QuitApp();

private:
	CFMUSim * m_cSim;

	tState m_eState;
	bool m_bStop;

	double m_dTime;
	double m_dStepSize;
	double m_dStepTolerance;
	double m_dStopTime;

	double m_dStepProgress;
	double m_dProgress;

	bool m_bUseTimer;
	boost::chrono::steady_clock::time_point m_ctSimTimeRef;
};

#endif // _RUNTHREAD_H_
