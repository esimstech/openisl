/*
 *     Name: isl_thread.cpp
 *
 *     Description: Thread tools.
 *
 *     Modification date: 19/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
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


