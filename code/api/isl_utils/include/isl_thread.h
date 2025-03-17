/*
 *     Name: isl_thread.h
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
