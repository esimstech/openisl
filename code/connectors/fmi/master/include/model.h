/*
 *     Name: model.h
 *
 *     Description: Model class.
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

#ifndef _MODEL_H_
#define _MODEL_H_

/*
 *     Header files
 */

#include <string>
#include <boost/filesystem.hpp>

#include <isl_xml.h>
#include <isl_api.h>

#include "fmusim_const.h"
#include "slave.h"
#include "modelvar.h"


/*
 *     Classes declaration
 */

/*
 *     Class CModel
 */

class CModel
{
public:
	enum tStatus {
		MODEL_STATUS_IDLE,
		MODEL_STATUS_LOADED
	};

	enum tFMIVersion {
		FMI_0_0,
		FMI_1_0,
		FMI_2_0
	};

	static const std::string GetStringFMIVersion(tFMIVersion eVersion);
	static const tFMIVersion GetEnumFMIVersion(const std::string & sVersion);

	CModel();
	~CModel();

	tStatus GetStatus();

	boost::filesystem::path GetFile();
	std::string GetDir();
	void SetKeepDir(bool bVal);
	void SetUseWorkingDir(bool bVal);
	void SetUseLogger(bool bVal);
	bool UseLogger();

	void SetSession(const std::string & sId);
	std::string GetSession();

	isl::CConnect * GetBlackBox();
	isl::CXML * GetFMU();
	CGenericSlave * GetSlave();

	void SetFMIVersion(tFMIVersion eVal);
	tFMIVersion GetFMIVersion();

	// TODO: Add an option from the command line
	void SetStore(bool bVal);
	bool GetStore();

	void SetSimSpeed(double dVal);
	bool IsSimSpeedValid();
	double GetSimSpeed();

	bool Validate(int & nErrorCode);

	bool Load(const std::string & sFile, bool bNewXML);
	// TODO: Add an option to save the XML file is requested
	bool Save(const std::string & sNewFile = "");

	CModelVars m_lVars;

	CModelVars m_lInitVars1rst;
	CModelVars m_lInitVars2nd;

	CModelVars m_lInputs;
	CModelVars m_lOutputs;

private:
	bool EraseAndDeleteOutputDir();

	boost::filesystem::path m_bfpFile;
	boost::filesystem::path * m_bfpDir;
	bool m_bKeepDir;
	bool m_bUseWorkingDir;
	bool m_bUseLogger;

	std::string m_sSession;

	isl::CConnect * m_cBlackBox;
	isl::CXML * m_cFMU;
	CGenericSlave * m_cSlave;

	tFMIVersion m_eFMIVersion;
	tStatus m_eStatus;

	bool m_bStore;
	double m_dSimSpeed;
};

#endif // _MODEL_H_