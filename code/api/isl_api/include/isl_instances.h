/*
 *     Name: isl_instances.h
 *
 *     Description: ISL connections instances management.
 *
 *     Modification date: 21/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_INSTANCES_H_
#define _ISL_INSTANCES_H_

/*
 *     Header files
 */

#include <string>
#include <vector>
#include <map>


/*
 *     Classes declaration
 */
 
namespace isl {
	class CConnect;

	class ISL_API_EXPORT CInstances
	{
	private:
		class CMngtInstances {
		private:
			CInstances * m_cInstance;

		public:
			CMngtInstances();
			~CMngtInstances();
			CInstances * Get();
		};

		CInstances();
		~CInstances();

	public:
		// Avoid a copy
		CInstances(const CInstances&) = delete;
		CInstances& operator=(const CInstances&) = delete;
		//
		static CInstances * Instance();

		void SetDeleteAll(bool bVal);
		bool GetDeleteAll();

		// If sUniqueId empty => id = session + "_" + "connector_id"
		int Add(CConnect * cConnect, const std::string & sUniqueId = "");
		bool Remove(const std::string & sUniqueId);
		bool Delete(const std::string & sUniqueId);

		int Size();
		CConnect * Get(const std::string& sUniqueId);

		void ClearAll();

		void SetSendStopSession(bool bVal);
		bool GetSendStopSession();

		void * AllocateMemory(size_t nNbElts, size_t nSizeElt);
		void * AllocateMemory(const std::string & sKey, size_t nNbElts, size_t nSizeElt);
		void * ReAllocateMemory(void * pOrgItem, size_t nNbElts, size_t nSizeElt);
		void FreeAllMemory();

	private:
		static CMngtInstances m_cMngtInstances;
		std::map<std::string, CConnect *> m_lConnectors;
		std::vector<void *> m_lMemoryItems;
		std::map<std::string, void *> m_mMemoryItems;

		bool m_bDeleteAll;
		bool m_bSendStopSession;
	};
}

#define ISLInstances isl::CInstances::Instance()

#endif // _ISL_INSTANCES_H_
