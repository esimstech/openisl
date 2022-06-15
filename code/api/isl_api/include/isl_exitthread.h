/*
 *     Name: isl_exitthread.h
 *
 *     Description: Thread used to exit an isl session.
 *
 *     Modification date: 10/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_EXITTHREAD_H_
#define _ISL_EXITTHREAD_H_

/*
 *     Header files
 */

#include <string>

#include <isl_thread.h>
#include <isl_sem.h>


/*
 *     Classes declaration
 */

namespace isl {
	class CConnect;

	class CExitThread : public CThread
	{
	public:
		static const std::string c_sKeyGeneralId;

		static bool SendStopRequest(const std::string & sSession = "");

		CExitThread(CConnect * cConnect, bool bMySession);
		~CExitThread();

	protected:
		void Run();

	private:
		CSem * m_cSem;
		CConnect * m_cConnect;
	};
}

#endif // _ISL_EXITTHREAD_H_
