/*
 *     Name: modelvar.h
 *
 *     Description: Model Variable class.
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

#ifndef _MODELVAR_H_
#define _MODELVAR_H_

/*
 *     Header files
 */

#include <vector>

#include "fmusim_const.h"
#include <isl_xml.h>
#include <isl_api.h>


/*
 *     Classes declaration
 */

class CModel;
class CGenericSlave;
class CModelVar
{
public:
	CModelVar(isl::CData * cIO, isl::CXMLNode * cVar, CModel * cParent);
	~CModelVar();

	isl::CData * GetIO();

	bool Validate(int & nErrorCode);

	bool InitializeModel();
	bool InitializeISL(double dTime);

	bool TransferDataISLToModel(double dTime);
	bool TransferDataModelToISL(double dTime);

private:
	isl::CData * m_cIO;

	isl::CXMLNode * m_cVar;
	unsigned int m_uRef;

	CGenericSlave * m_cSlave;

	isl::CDataType::tType m_eType;
	int m_nSize;
	bool m_bStore;

	void * m_pData;
};

typedef std::vector<CModelVar *> CModelVars;

#endif // _MODELVAR_H_
