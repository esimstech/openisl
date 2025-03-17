/*
 *     Name: isl_thread.cpp
 *
 *     Description: Thread tools.
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

#include <boost/chrono.hpp>

#include "isl_thread.h"


/*
 *     Classes definition
 */

void isl::CThread::Sleep(unsigned long ulMilliSec)
{
	boost::this_thread::sleep_for(boost::chrono::milliseconds(ulMilliSec));
}

isl::CThread::CThread()
{
	m_eStatus = isl::CThread::THREAD_IDLE;
}

isl::CThread::~CThread()
{
}

bool isl::CThread::Start()
{
	m_cTh = boost::thread(&isl::CThread::Run, this);
	m_eStatus = isl::CThread::THREAD_RUNNING;
	return true;
}

void isl::CThread::Lock()
{
	m_cLock.lock();
}

void isl::CThread::Unlock()
{
	m_cLock.unlock();
}

void isl::CThread::Join()
{
	m_cTh.join();
}

void isl::CThread::Detach()
{
	m_cTh.detach();
}

void isl::CThread::Interrupt()
{
	m_cTh.interrupt();
}


