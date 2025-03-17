/*
 *     Name: model.cpp
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

/*
 *     Header files
 */

#include <boost/format.hpp>

#include <isl_log.h>
#include <isl_xml.h>

#include "fmusim_const.h"
#include "appsettings.h"
#include "slave_v2_0.h"
#include "utils.h"
#include "model.h"


/*
 *     Classes definition
 */

/*
 *     Class CModel
 */

const std::string CModel::GetStringFMIVersion(tFMIVersion eVersion)
{
	switch (eVersion) {
		case FMI_1_0:
			return std::string("1.0");
		case FMI_2_0:
			return std::string("2.0");
		case FMI_0_0:
		default:
			break;
	}
	return std::string();
}

const CModel::tFMIVersion CModel::GetEnumFMIVersion(const std::string & sVersion)
{
	if (sVersion == "1.0") {
		return FMI_1_0;
	}
	else if (sVersion == "2.0") {
		return FMI_2_0;
	}
	return FMI_0_0;
}

CModel::CModel()
{
	m_bfpDir = 0;
	m_bKeepDir = false;
	m_bUseWorkingDir = false;
	m_bUseLogger = false;
	//
	m_cBlackBox = 0;
	m_cFMU = 0;
	m_cSlave = 0;
	//
	m_eFMIVersion = FMI_0_0;
	m_eStatus = MODEL_STATUS_IDLE;
	//
	m_bStore = false;
	m_dSimSpeed = 0.0;
}

CModel::~CModel()
{
	AppLogInfo(INFO_MODEL_UNLOADING, "Unloading the model.");
	while (m_lVars.empty() == false) {
		delete m_lVars.back();
		m_lVars.pop_back();
	}
	m_lInitVars1rst.clear();
	m_lInitVars2nd.clear();
	m_lInputs.clear();
	m_lOutputs.clear();
	if (m_cSlave != 0) {
		delete m_cSlave;
		// Will automatically unload the library if it is loaded.
	}
	m_cSlave = 0;
	if (m_cBlackBox != 0) {
		delete m_cBlackBox;
	}
	m_cBlackBox = 0;
	if (m_cFMU != 0) {
		delete m_cFMU;
	}
	m_cFMU = 0;
	// Delete the temporary directory and its contents
	if (m_bfpDir != 0) {
		EraseAndDeleteOutputDir();
	}
	m_bfpDir = 0;
	//
	m_eFMIVersion = FMI_0_0;
	m_eStatus = MODEL_STATUS_IDLE;
	//
	m_bStore = false;
}

CModel::tStatus CModel::GetStatus()
{
	return m_eStatus;
}

boost::filesystem::path CModel::GetFile()
{
	return m_bfpFile;
}

std::string CModel::GetDir()
{
	if (m_bfpDir != 0) {
		return m_bfpDir->string();
	}
	return std::string();
}

void CModel::SetKeepDir(bool bVal)
{
	m_bKeepDir = bVal;
}

void CModel::SetUseWorkingDir(bool bVal)
{
	m_bUseWorkingDir = bVal;
}

void CModel::SetUseLogger(bool bVal)
{
	m_bUseLogger = bVal;
}

bool CModel::UseLogger()
{
	return m_bUseLogger;
}

void CModel::SetSession(const std::string & sId)
{
	m_sSession = sId;
}

std::string CModel::GetSession()
{
	return m_sSession;
}

isl::CConnect * CModel::GetBlackBox()
{
	return m_cBlackBox;
}

isl::CXML * CModel::GetFMU()
{
	return m_cFMU;
}

CGenericSlave * CModel::GetSlave()
{
	return m_cSlave;
}

void CModel::SetFMIVersion(tFMIVersion eVal)
{
	m_eFMIVersion = eVal;
}

CModel::tFMIVersion CModel::GetFMIVersion()
{
	return m_eFMIVersion;
}

void CModel::SetStore(bool bVal)
{
	m_bStore = bVal;
}

bool CModel::GetStore()
{
	return m_bStore;
}

void CModel::SetSimSpeed(double dVal)
{
	m_dSimSpeed = dVal;
}

bool CModel::IsSimSpeedValid()
{
	return ((m_dSimSpeed > 0.0) && (m_dSimSpeed <= 20.0));
}

double CModel::GetSimSpeed()
{
	return m_dSimSpeed;
}

bool CModel::Validate(int & nErrorCode)
{
	if (m_cFMU == 0) {
		nErrorCode = 10;
		return false;
	}
	if (m_cBlackBox == 0) {
		nErrorCode = 11;
		return false;
	}
	if (m_cSlave == 0) {
		nErrorCode = 12;
		return false;
	}
	bool bOk = true;
	if (m_lVars.empty() == false) {
		CModelVars::iterator iVar;
		for (iVar = m_lVars.begin(); iVar != m_lVars.end(); ++iVar) {
			if ((*iVar)->Validate(nErrorCode) == false) {
				bOk = false;
			}
		}
	}
	if (m_sSession.empty()) {
		nErrorCode = 13;
		return false;
	}
	return bOk;
}

bool CModel::Load(const std::string & sFile, bool bNewXML)
{
	if (m_eStatus != MODEL_STATUS_IDLE) {
		AppLogWarning(WARNING_MODEL_MODELLOADED, "A model is already loaded.");
		return false;
	}
	m_bfpFile = sFile;
	// Check if the file exists
	if (boost::filesystem::exists(m_bfpFile) == false) {
		AppLogError(ERROR_MODEL_FILENOTEXIST,
			"Model file '%s' doesn't exist.", m_bfpFile.string().c_str());
		return false;
	}
	// Make the file absolute
	AppLogInfo(INFO_MODEL_LOADING, "Loading file '%s'...", m_bfpFile.string().c_str());
	//
	// Step #1: Unzip the FMU model
	//
	// Create the output directory
	if (m_bfpDir != 0) {
		// Clean the output directory
		EraseAndDeleteOutputDir();
	}
	if (m_bUseWorkingDir) {
		m_bfpDir = new boost::filesystem::path(boost::filesystem::absolute(boost::filesystem::unique_path()));
	}
	else {
		boost::filesystem::path bfpTemp(boost::filesystem::temp_directory_path());
		bfpTemp.append(boost::filesystem::unique_path().string());
		m_bfpDir = new boost::filesystem::path(bfpTemp);
	}
	AppLogInfo(INFO_MODEL_CREATEDIR, "Creating a new directory: %s", m_bfpDir->string().c_str());
	if (boost::filesystem::create_directory(*m_bfpDir) == false) {
		AppLogError(ERROR_MODEL_DIRNOTCREATED, "Failed to create a temporary directory.");
		delete m_bfpDir;
		m_bfpDir = 0;
		return false;
	}
	// Extract the files using the ZipCmd of the application or the default ISL zip tool
	std::string sZipCmd = CExeSettings().GetZipCmd();
	if (sZipCmd.empty()) {
		sZipCmd = boost::str(boost::format(DEFAULT_ZIPCMD) % isl::CUtils::GetISLPath()
			% boost::filesystem::absolute(m_bfpFile).string() % m_bfpDir->string());
	}
	else {
		sZipCmd = boost::str(boost::format(sZipCmd)
			% boost::filesystem::absolute(m_bfpFile).string() % m_bfpDir->string());
	}
	AppLogInfo(INFO_MODEL_RUNNINGZIPCMD, "Running the command: %s...", sZipCmd.c_str());
	int nRetCode = CAppUtils::RunCommand(sZipCmd);
	// Get the model description file
	boost::filesystem::path bfpModelDesc(*m_bfpDir);
	bfpModelDesc.append("modelDescription.xml");
	if (boost::filesystem::exists(bfpModelDesc) == false) {
		AppLogError(ERROR_MODEL_MODELDESC_NOTEXIST,
			"Model description file '%s' doesn't exist.", bfpModelDesc.string().c_str());
		EraseAndDeleteOutputDir();
		return false;
	}
	std::string sModelDesc(bfpModelDesc.string());
	//
	// Step #2: Load the model description
	//
	if (m_cFMU != 0) {
		AppLogInfo(INFO_MODEL_UNLOADINGPREVFMU, "Unloading the previous FMU model...");
		delete m_cFMU;
		m_cFMU = 0;
	}
	m_cFMU = new isl::CXML(sModelDesc);
	if (m_cFMU->Load() == false) {
		AppLogError(ERROR_MODEL_LOADMODELDESC, "Error on loading '%s': %d.",
			sModelDesc.c_str(), m_cFMU->GetStatus());
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	if (m_cFMU->Parse() == false) {
		AppLogError(ERROR_MODEL_PARSEMODELDESC, "Error on parsing '%s':\n\t%d: %s.",
			sModelDesc.c_str(), m_cFMU->GetStatus(), m_cFMU->GetParserError().c_str());
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	//
	// Step #3: Load the black box file (ISL XML file)
	//
	if (m_cBlackBox != 0) {
		AppLogInfo(INFO_MODEL_UNLOADINGPREVXML, "Unloading the previous XML model file...");
		delete m_cBlackBox;
		m_cBlackBox = 0;
	}
	boost::filesystem::path bfpXML(m_bfpFile);
	bfpXML.replace_extension("xml");
	m_cBlackBox = new isl::CConnect();
	bool bOk = true;
	if (boost::filesystem::exists(bfpXML) == false) {
		if (bNewXML) {
			if (m_cBlackBox->New(bfpXML.string()) == false) {
				AppLogError(ERROR_MODEL_FAILEDXMLCREATE,
					"Failed to create a valid new XML model file.");
				bOk = false;
			}
			// The XML file must be filled later
		}
		else {
			// No file to load
			AppLogError(ERROR_MODEL_XMKFILENOTEXIST,
				"The file '%s' doesn't exists.", bfpXML.string().c_str());
			bOk = false;
		}
	}
	else {
		if (m_cBlackBox->Load(bfpXML.string()) == false) {
			AppLogError(ERROR_MODEL_LOADXMLFILE, "Error on loading XML file '%s'",
				bfpXML.string().c_str());
			bOk = false;
		}
	}
	if (bOk == false) {
		delete m_cBlackBox;
		m_cBlackBox = 0;
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	if (m_cBlackBox->Check() == false) {
		AppLogError(ERROR_MODEL_CHECKMODEL, "Error on checking model '%s'",
			m_cBlackBox->GetName().c_str());
		delete m_cBlackBox;
		m_cBlackBox = 0;
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	// If session already set, use it
	if (m_cBlackBox->GetSessionId().empty() == false) {
		SetSession(m_cBlackBox->GetSessionId());
	}
	// Check if we use the 'store' option
#if 0 // TODO: To implement
	m_bStore = m_cBlackBox->UseStore();
#endif
	if (m_bStore) {
		AppLogInfo(INFO_MODEL_STORAGEMODE, "Storage mode enabled.");
	}
	//
	// Step #4: Create the slave model
	//
	std::string sFMIVersion;
	try {
		sFMIVersion = m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetAttribute("fmiVersion");
	}
	catch (...) {
		AppLogError(ERROR_MODEL_FMUXML_E1, "Wrong XML format in the FMU description file.");
		delete m_cBlackBox;
		m_cBlackBox = 0;
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	m_eFMIVersion = CModel::GetEnumFMIVersion(sFMIVersion);
	if (m_eFMIVersion == FMI_2_0) {
		m_cSlave = new CSlave2_0();
		m_cSlave->SetModel(this);
	}
	else {
		AppLogError(ERROR_MODEL_NOTVALIDFMIVERSION,
			"Not a valid FMI version: %s. Please load a model 2.0 compatible.", sFMIVersion.c_str());
		delete m_cBlackBox;
		m_cBlackBox = 0;
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	//
	// Step #5: Build the I/Os
	//
	isl::CXMLNode * cxVars = 0;
	try {
		cxVars = m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetNode("ModelVariables");
	}
	catch (...) {
		AppLogError(ERROR_MODEL_FMUXML_E2, "Wrong XML format in the FMU description file.");
		delete m_cBlackBox;
		m_cBlackBox = 0;
		delete m_cFMU;
		m_cFMU = 0;
		EraseAndDeleteOutputDir();
		return false;
	}
	bool bVarOk = true;
	int nNbIOs = m_cBlackBox->GetNbIOs();
	if (nNbIOs > 0) {
		for (int i = 0; i < nNbIOs; i++) {
			isl::CData * cIO = m_cBlackBox->GetIO(i);
			if (cIO == NULL) {
				continue;
			}
			if (cIO->GetConnectId().empty() == true) {
				continue;
			}
			isl::CXMLNode * cxVar = cxVars->GetNode("ScalarVariable", "name", cIO->GetId());
			if (cxVar == NULL) {
				AppLogWarning(WARNING_MODEL_NOVARFOUND,
					"No FMU variable with the name %s have been found.", cIO->GetId().c_str());
				continue;
			}
			CModelVar * cModelVar = new CModelVar(cIO, cxVar, this);
			int nErrorCode = 0;
			if (cModelVar->Validate(nErrorCode) == false) {
				AppLogError(ERROR_MODEL_VARNOTVALID, "'%s' is not a valid variable.",
					cIO->GetId().c_str());
				delete cModelVar;
				bVarOk = false;
				continue;
			}
			m_lVars.push_back(cModelVar);
			// Get variables to initialize
			if (cxVar->GetAttribute("variability") != "constant") {
				if (cxVar->GetAttribute("causality") == "input") {
					m_lInitVars2nd.push_back(cModelVar);
				}
				else {
					std::string sInitial(cxVar->GetAttribute("initial"));
					if (sInitial == "exact") {
						m_lInitVars2nd.push_back(cModelVar);
					} else if (sInitial == "approx") {
						m_lInitVars1rst.push_back(cModelVar);
					}
				}
			}
			// Get inputs
			if (cIO->IsInput()) {
				m_lInputs.push_back(cModelVar);
			}
			// Get outputs
			if (cIO->IsOutput()) {
				// Note: variable values of "local" causality should NOT be used by other models
				m_lOutputs.push_back(cModelVar);
			}
		}
	}
	if (bVarOk == false) {
		return false;
	}
	//
	AppLogInfo(INFO_MODEL_LOADED,
		"Model '%s' has been loaded.", m_bfpFile.string().c_str());
	m_eStatus = MODEL_STATUS_LOADED;
	return true;
}

bool CModel::Save(const std::string & sNewFile)
{
	if ((m_eStatus != MODEL_STATUS_LOADED) || (m_cBlackBox == 0)) {
		AppLogWarning(WARNING_MODEL_SAVENOMODEL,
			"Save failed. Please load a model first.");
		return false;
	}
	if (m_cBlackBox->Save(sNewFile) == false) {
		AppLogError(ERROR_MODEL_SAVEFAILED, "Failed to save the file '%s'.",
			m_cBlackBox->GetFileName().c_str());
		return false;
	}
	//
	AppLogInfo(INFO_MODEL_SAVED, "Model '%s' has been saved.",
		m_cBlackBox->GetFileName().c_str());
	return true;
}

bool CModel::EraseAndDeleteOutputDir()
{
	AppLogInfo(INFO_MODEL_CLEANDIR,
		"Cleaning the existing directory: %s", m_bfpDir->string().c_str());
	if (m_bKeepDir) {
		AppLogWarning(WARNING_MODEL_TMPDIRNOTREMOVED,
			"The directory '%s' won't be removed. Please remove it manually.",
			m_bfpDir->string().c_str());
	}
	else {
		AppLogInfo(INFO_MODEL_REMOVETMPDIR, "Deleting the temporary directory '%s'...",
			m_bfpDir->string().c_str());
		boost::filesystem::remove_all(*m_bfpDir);
	}
	delete m_bfpDir;
	m_bfpDir = 0;
	return true;
}