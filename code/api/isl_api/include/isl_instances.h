/*
 *     Name: isl_instances.h
 *
 *     Description: ISL connections instances management.
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
