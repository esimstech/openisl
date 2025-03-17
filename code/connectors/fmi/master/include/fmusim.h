/*
 *     Name: fmusim.h
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

#ifndef _FMUSIM_H_
#define _FMUSIM_H_

/*
 *     Header files
 */

#include "model.h"
#include "runthread.h"


/*
 *     Classes declaration
 */

class CFMUSim
{
public:
	CFMUSim(CModel * cModel);
	~CFMUSim();

	bool Run();
	bool Stop();
	bool Wait();

	bool IsRunning();
	bool IsReadyToRun(int & nErrorCode);

	void SetSimulationRate(double dVal);
	//void SetProgress(double dVal);

	CModel * GetModel();

private:
	double m_dSimulationRate;

	CModel * m_cModel;
	CRunThread * m_cRunThread;
};

#endif // _FMUSIM_H_
