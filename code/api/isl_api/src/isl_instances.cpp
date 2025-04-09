/*
 *     Name: isl_instances.cpp
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

/*
 *     Header files
 */

#include "isl_api.h"


/*
 *     Classes definition
 */

/*
 *     Class CInstances
 */

isl::CInstances::CMngtInstances isl::CInstances::m_cMngtInstances;

isl::CInstances::CMngtInstances::CMngtInstances()
{
	m_cInstance = new CInstances;
}

isl::CInstances::CMngtInstances::~CMngtInstances()
{
	delete m_cInstance;
}

isl::CInstances * isl::CInstances::CMngtInstances::Get()
{
	return m_cInstance;
}

isl::CInstances::CInstances()
{
	m_bDeleteAll = false;
	m_bSendStopSession = false;
}

isl::CInstances::~CInstances()
{
	if (m_bDeleteAll) {
		ClearAll();
	}
	FreeAllMemory();
}

isl::CInstances * isl::CInstances::Instance()
{
	return m_cMngtInstances.Get();
}

void isl::CInstances::SetDeleteAll(bool bVal)
{
	m_bDeleteAll = bVal;
}

bool isl::CInstances::GetDeleteAll()
{
	return m_bDeleteAll;
}

int isl::CInstances::Add(CConnect * cConnect, std::string & sUniqueId)
{
	std::string sId = sUniqueId;
	if (sId.empty() && cConnect) {
		sId = cConnect->GetSessionId() + "_" + cConnect->GetId();
		sUniqueId = sId;
	}
	if (cConnect) {
		m_lConnectors[sId] = cConnect;
		return m_lConnectors.size() - 1;
	}
	return -1;
}

int isl::CInstances::Add(CConnect* cConnect)
{
	std::string sId;
	return Add(cConnect, sId);
}

bool isl::CInstances::Remove(const std::string & sUniqueId)
{
	if (sUniqueId.empty()) {
		return false;
	}
	std::map<std::string, CConnect *>::iterator iConnect = m_lConnectors.find(sUniqueId);
	if (iConnect != m_lConnectors.end()) {
		m_lConnectors.erase(iConnect);
		return true;
	}
	return false;
}

bool isl::CInstances::Delete(const std::string & sUniqueId)
{
	if (sUniqueId.empty()) {
		return false;
	}
	std::map<std::string, CConnect *>::iterator iConnect = m_lConnectors.find(sUniqueId);
	if (iConnect != m_lConnectors.end()) {
		delete iConnect->second;
		m_lConnectors.erase(iConnect);
		return true;
	}
	return false;
}

int isl::CInstances::Size()
{
	return m_lConnectors.size();
}

isl::CConnect * isl::CInstances::Get(const std::string& sUniqueId)
{
	try {
		return m_lConnectors.at(sUniqueId);
	}
	catch (...) {
		return 0;
	}
}

void isl::CInstances::ClearAll()
{
	std::map<std::string, CConnect *>::iterator iConnect = m_lConnectors.begin();
	while (iConnect != m_lConnectors.end()) {
		if (m_bSendStopSession) {
			iConnect->second->SendStopSession();
		}
		delete iConnect->second;
		iConnect = m_lConnectors.erase(iConnect);
	}
}

void isl::CInstances::SetSendStopSession(bool bVal)
{
	m_bSendStopSession = bVal;
}

bool isl::CInstances::GetSendStopSession()
{
	return m_bSendStopSession;
}

void * isl::CInstances::AllocateMemory(size_t nNbElts, size_t nSizeElt)
{
	void * pItem = calloc(nNbElts, nSizeElt);
	if (pItem != 0) {
		m_lMemoryItems.push_back(pItem);
	}
	return pItem;
}

void * isl::CInstances::AllocateMemory(const std::string & sKey, size_t nNbElts, size_t nSizeElt)
{
	void * pItem = 0;
	try {
		pItem = m_mMemoryItems.at(sKey);
		pItem = ReAllocateMemory(pItem, nNbElts, nSizeElt);
	}
	catch (...) {
		pItem = AllocateMemory(nNbElts, nSizeElt);
	}
	return pItem;
}

void * isl::CInstances::ReAllocateMemory(void * pOrgItem, size_t nNbElts, size_t nSizeElt)
{
	if (pOrgItem == 0) {
		return AllocateMemory(nNbElts, nSizeElt);
	}
	bool bFound = false;
	std::vector<void *>::iterator iItem;
	for (iItem = m_lMemoryItems.begin(); iItem != m_lMemoryItems.end(); ++iItem) {
		if (*iItem == pOrgItem) {
			m_lMemoryItems.erase(iItem);
			break;
		}
	}
	if (bFound) {
		void * pItem = realloc(pOrgItem, nNbElts * nSizeElt);
		if (pItem != 0) {
			m_lMemoryItems.push_back(pItem);
		}
		return pItem;
	}
	return AllocateMemory(nNbElts, nSizeElt);
}

void isl::CInstances::FreeAllMemory()
{
	std::vector<void *>::iterator iItem = m_lMemoryItems.begin();
	while (iItem != m_lMemoryItems.end()) {
		free(*iItem);
		iItem = m_lMemoryItems.erase(iItem);
	}
	m_mMemoryItems.clear();
}
