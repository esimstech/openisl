/*
 *     Name: isl_thread.h
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

#ifndef _ISL_THREAD_H_
#define _ISL_THREAD_H_

 /*
  *     Header files
  */

#include <boost/thread.hpp>
#include <boost/thread/mutex.hpp>


   /*
	*     Classes declaration
	*/

namespace isl {
	class CThread {
	public:
		enum tStatus {
			THREAD_IDLE = 0,
			THREAD_RUNNING,
			THREAD_STOPPED
		};

		static void Sleep(unsigned long ulMilliSec);

		CThread();
		~CThread();

		bool Start();
	protected:
		virtual void Run() {}

	public:
		tStatus GetStatus() { return m_eStatus; }

		void Lock();
		void Unlock();

		void Join();
		void Detach();

		void Interrupt();

	protected:
		tStatus m_eStatus;

	private:
		boost::thread m_cTh;
		boost::mutex m_cLock;
	};
}


#endif // _ISL_THREAD_H_
