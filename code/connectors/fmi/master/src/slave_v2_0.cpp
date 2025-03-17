/* 
 *     Name: slave_v2_0.cpp
 *
 *     Description: Class CSlave2_0: FMU 2.0 slave model.
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

#include "fmusim_const.h"
#include <v2.0/fmi2Functions.h>

#include <isl_misc.h>
#include <isl_log.h>

#include "model.h"
#include "slave_v2_0.h"


/*
 *     Macros and constants definition
 */

#define FCT_FMIINSTANTIATE				"fmi2Instantiate"
#define FCT_FMISETDEBUGLOGGING			"fmi2SetDebugLogging"
#define FCT_FMISETUPEXPERIMENT			"fmi2SetupExperiment"
#define FCT_FMIENTERINITIALIZATIONMODE	"fmi2EnterInitializationMode"
#define FCT_FMIEXITINITIALIZATIONMODE	"fmi2ExitInitializationMode"
#define FCT_FMIDOSTEP					"fmi2DoStep"
#define FCT_FMISETREAL					"fmi2SetReal"
#define FCT_FMIGETREAL					"fmi2GetReal"
#define FCT_FMISETINTEGER				"fmi2SetInteger"
#define FCT_FMIGETINTEGER				"fmi2GetInteger"
#define FCT_FMISETBOOLEAN				"fmi2SetBoolean"
#define FCT_FMIGETBOOLEAN				"fmi2GetBoolean"
#define FCT_FMISETSTRING				"fmi2SetString"
#define FCT_FMIGETSTRING				"fmi2GetString"
#define FCT_FMIGETSTATUS				"fmi2GetStatus"
#define FCT_FMIGETBOOLEANSTATUS			"fmi2GetBooleanStatus"
#define FCT_FMITERMINATE				"fmi2Terminate"
#define FCT_FMIRESET					"fmi2Reset"
#define FCT_FMIFREEINSTANCE				"fmi2FreeInstance"


/*
 *     Classes definition
 */

/*
 *     Class CSlaveFunctions2_0
 */

class CSlaveFunctions2_0
{
public:
	typedef fmi2Component (*tInstantiate) (fmi2String instanceName, fmi2Type fmuType,
		fmi2String fmuGUID, fmi2String  fmuResourceLocation, const fmi2CallbackFunctions * functions,
		fmi2Boolean visible, fmi2Boolean loggingOn);
	typedef fmi2Status (*tSetDebugLogging) (fmi2Component c, fmi2Boolean loggingOn,
		size_t nCategories, const fmi2String categories[]);
	typedef fmi2Status (*tSetupExperiment) (fmi2Component c, fmi2Boolean toleranceDefined,
		fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime);
	typedef fmi2Status (*tEnterInitializationMode) (fmi2Component c);
	typedef fmi2Status (*tExitInitializationMode) (fmi2Component c);
	typedef fmi2Status (*tDoStep) (fmi2Component c,
		fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint);
	typedef fmi2Status (*tSetReal) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2Real value[]);
	typedef fmi2Status (*tGetReal) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2Real value[]);
	typedef fmi2Status (*tSetInteger) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2Integer value[]);
	typedef fmi2Status (*tGetInteger) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2Integer value[]);
	typedef fmi2Status (*tSetBoolean) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2Boolean value[]);
	typedef fmi2Status (*tGetBoolean) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2Boolean value[]);
	typedef fmi2Status (*tSetString) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2String value[]);
	typedef fmi2Status (*tGetString) (fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2String value[]);
	typedef fmi2Status (*tGetStatus) (fmi2Component c, const fmi2StatusKind s,
		fmi2Status * value);
	typedef fmi2Status (*tGetBooleanStatus) (fmi2Component c, const fmi2StatusKind s,
		fmi2Boolean * value);
	typedef fmi2Status (*tTerminate) (fmi2Component c);
	typedef fmi2Status (*tReset) (fmi2Component c);
	typedef void (*tFreeInstance) (fmi2Component c);

	tInstantiate Instantiate;
	tSetDebugLogging SetDebugLogging;
	tSetupExperiment SetupExperiment;
	tEnterInitializationMode EnterInitializationMode;
	tExitInitializationMode ExitInitializationMode;
	tDoStep DoStep;
	tSetReal SetReal;
	tGetReal GetReal;
	tSetInteger SetInteger;
	tGetInteger GetInteger;
	tSetBoolean SetBoolean;
	tGetBoolean GetBoolean;
	tSetString SetString;
	tGetString GetString;
	tGetStatus GetStatus;
	tGetBooleanStatus GetBooleanStatus;
	tTerminate Terminate;
	tReset Reset;
	tFreeInstance FreeInstance;

	static void Logger(fmi2ComponentEnvironment c, fmi2String instanceName, fmi2Status status,
		fmi2String category, fmi2String message, ...);
	static void StepFinished(fmi2ComponentEnvironment c, fmi2Status status);

	static fmi2CallbackFunctions m_fCallbacks;

	static CSlaveFunctions2_0 * Instance();

	bool m_bUseLogger;

protected:
	CSlaveFunctions2_0();
	~CSlaveFunctions2_0() {};

private:
	static CSlaveFunctions2_0 * m_cInstance;
};

#define SlaveFcts CSlaveFunctions2_0::Instance()

CSlaveFunctions2_0 * CSlaveFunctions2_0::m_cInstance = 0;

fmi2CallbackFunctions CSlaveFunctions2_0::m_fCallbacks = { CSlaveFunctions2_0::Logger, calloc, free,
CSlaveFunctions2_0::StepFinished, 0 };


CSlaveFunctions2_0 * CSlaveFunctions2_0::Instance()
{
	if (m_cInstance == 0) {
		m_cInstance = new CSlaveFunctions2_0();
	}
	return m_cInstance;
}

CSlaveFunctions2_0::CSlaveFunctions2_0()
{
	Instantiate = 0;
	SetDebugLogging = 0;
	SetupExperiment = 0;
	EnterInitializationMode = 0;
	ExitInitializationMode = 0;
	DoStep = 0;
	SetReal = 0;
	GetReal = 0;
	SetInteger = 0;
	GetInteger = 0;
	SetBoolean = 0;
	GetBoolean = 0;
	SetString = 0;
	GetString = 0;
	GetStatus = 0;
	GetBooleanStatus = 0;
	Terminate = 0;
	Reset = 0;
	FreeInstance = 0;
	m_bUseLogger = false;
}

void CSlaveFunctions2_0::Logger(fmi2ComponentEnvironment c, fmi2String instanceName,
	fmi2Status status, fmi2String category, fmi2String message, ...)
{
	if (SlaveFcts->m_bUseLogger == false) {
		return;
	}
	va_list lArgs;
	va_start(lArgs, message);
	size_t len = std::vsnprintf(NULL, 0, message, lArgs);
	va_end(lArgs);
	std::vector<char> sVec(len + 1);
	va_start(lArgs, message);
	std::vsnprintf(&sVec[0], len + 1, message, lArgs);
	va_end(lArgs);
	std::string sPrefixMsg = boost::str(boost::format("[%1%] %2% (%3%): ")
		% status % (instanceName == 0 ? "unknown" : instanceName)
		% (category == 0 ? "unknown" : category));
	std::string sMsg = sPrefixMsg + &sVec[0];
	switch (status) {
		case fmi2Warning:
		case fmi2Discard:
			AppLogWarning(WARNING_SLAVE2_LOGGER, "%s", sMsg.c_str());
			break;
		case fmi2Error:
		case fmi2Fatal:
			AppLogError(ERROR_SLAVE2_LOGGER, "%s", sMsg.c_str());
			break;
		case fmi2Pending:
		case fmi2OK:
		default:
			AppLogInfo(INFO_SLAVE2_LOGGER, "%s", sMsg.c_str());
			break;
	}
}

void CSlaveFunctions2_0::StepFinished(fmi2ComponentEnvironment c, fmi2Status status)
{
	// Function used when fmi2DoStep returns fmi2Pending:
	// fmi2Pending is returned if the slave executes the function asynchronously. That means
	// the slave starts the computation but returns immediately.The master has to call
	// fmi2GetStatus(..., fmi2DoStep, ...) to find out, if the slave is done.
	//
	if (c == 0) {
		return;
	}
	switch (status) {
		case fmi2OK:
			break;
		case fmi2Pending:
			while (status == fmi2Pending) {
				if (SlaveFcts->GetStatus(((CSlave2_0 *)c)->GetComponent(),
						fmi2DoStepStatus, &status) != fmi2OK) {
					break;
				}
			}
			if (status == fmi2OK) {
				break;
			}
			((CSlave2_0 *)c)->SetErrorCode(CGenericSlave::SLAVE_ERROR_ASYNCSIM_FAILED);
			((CSlave2_0 *)c)->SetStatus(CGenericSlave::SLAVE_STATUS_ERROR);
			break;
		case fmi2Discard:
		{
			fmi2Boolean bVal = fmi2False;
			SlaveFcts->GetBooleanStatus(((CSlave2_0 *)c)->GetComponent(), fmi2Terminated, &bVal);
			if (bVal == fmi2True) {
				((CSlave2_0 *)c)->SetErrorCode(CGenericSlave::SLAVE_ERROR_STOP_REQUIRED);
				((CSlave2_0 *)c)->SetStatus(CGenericSlave::SLAVE_STATUS_ERROR);
				break;
			}
		}
		case fmi2Error:
		default:
			AppLogError(ERROR_SLAVE2_SIMFAILED, "Failed to complete simulation of the model.");
			((CSlave2_0 *)c)->SetErrorCode(CGenericSlave::SLAVE_ERROR_SIMULATION_FAILED);
			((CSlave2_0 *)c)->SetStatus(CGenericSlave::SLAVE_STATUS_ERROR);
			break;
	}
	((CSlave2_0 *)c)->Unlock();
}

/*
 *     Class CFMUSlaveV2
 */

CSlave2_0::CSlave2_0()
{
}

CSlave2_0::~CSlave2_0()
{
}

bool CSlave2_0::MapFunctions()
{
	if (LoadLibrary() == false) {
		return false;
	}
	std::string sFunction;
	sFunction = FCT_FMIINSTANTIATE;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIINSTANTIATE ".";
		return false;
	}
	SlaveFcts->Instantiate = m_bdLibrary->get<fmi2Component(fmi2String instanceName, fmi2Type fmuType,
		fmi2String fmuGUID, fmi2String  fmuResourceLocation, const fmi2CallbackFunctions * functions,
		fmi2Boolean visible, fmi2Boolean loggingOn)>(sFunction);
	sFunction = FCT_FMISETDEBUGLOGGING;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMISETDEBUGLOGGING ".";
		return false;
	}
	SlaveFcts->SetDebugLogging = m_bdLibrary->get<fmi2Status(fmi2Component c, fmi2Boolean loggingOn,
		size_t nCategories, const fmi2String categories[])>(sFunction);
	sFunction = FCT_FMISETUPEXPERIMENT;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMISETUPEXPERIMENT ".";
		return false;
	}
	SlaveFcts->SetupExperiment = m_bdLibrary->get<fmi2Status(fmi2Component c, fmi2Boolean toleranceDefined,
		fmi2Real tolerance, fmi2Real startTime, fmi2Boolean stopTimeDefined, fmi2Real stopTime)>(sFunction);
	sFunction = FCT_FMIENTERINITIALIZATIONMODE;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIENTERINITIALIZATIONMODE ".";
		return false;
	}
	SlaveFcts->EnterInitializationMode = m_bdLibrary->get<fmi2Status(fmi2Component c)>(sFunction);
	sFunction = FCT_FMIEXITINITIALIZATIONMODE;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIEXITINITIALIZATIONMODE ".";
		return false;
	}
	SlaveFcts->ExitInitializationMode = m_bdLibrary->get<fmi2Status(fmi2Component c)>(sFunction);
	sFunction = FCT_FMIDOSTEP;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIDOSTEP ".";
		return false;
	}
	SlaveFcts->DoStep = m_bdLibrary->get<fmi2Status(fmi2Component c,
		fmi2Real currentCommunicationPoint, fmi2Real communicationStepSize,
		fmi2Boolean noSetFMUStatePriorToCurrentPoint)>(sFunction);
	sFunction = FCT_FMISETREAL;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMISETREAL ".";
		return false;
	}
	SlaveFcts->SetReal = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2Real value[])>(sFunction);
	sFunction = FCT_FMIGETREAL;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIGETREAL ".";
		return false;
	}
	SlaveFcts->GetReal = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2Real value[])>(sFunction);
	sFunction = FCT_FMISETINTEGER;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMISETINTEGER ".";
		return false;
	}
	SlaveFcts->SetInteger = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2Integer value[])>(sFunction);
	sFunction = FCT_FMIGETINTEGER;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIGETINTEGER ".";
		return false;
	}
	SlaveFcts->GetInteger = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2Integer value[])>(sFunction);
	sFunction = FCT_FMISETBOOLEAN;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMISETBOOLEAN ".";
		return false;
	}
	SlaveFcts->SetBoolean = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2Boolean value[])>(sFunction);
	sFunction = FCT_FMIGETBOOLEAN;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIGETBOOLEAN ".";
		return false;
	}
	SlaveFcts->GetBoolean = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2Boolean value[])>(sFunction);
	sFunction = FCT_FMISETSTRING;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMISETSTRING ".";
		return false;
	}
	SlaveFcts->SetString = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, const fmi2String value[])>(sFunction);
	sFunction = FCT_FMIGETSTRING;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIGETSTRING ".";
		return false;
	}
	SlaveFcts->GetString = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2ValueReference vr[],
		size_t nvr, fmi2String value[])>(sFunction);
	sFunction = FCT_FMIGETSTATUS;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIGETSTATUS ".";
		return false;
	}
	SlaveFcts->GetStatus = m_bdLibrary->get<fmi2Status(fmi2Component c,
		const fmi2StatusKind s, fmi2Status * value)>(sFunction);
	sFunction = FCT_FMIGETBOOLEANSTATUS;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIGETBOOLEANSTATUS ".";
		return false;
	}
	SlaveFcts->GetBooleanStatus = m_bdLibrary->get<fmi2Status(fmi2Component c, const fmi2StatusKind s,
		fmi2Boolean * value)>(sFunction);
	sFunction = FCT_FMITERMINATE;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMITERMINATE ".";
		return false;
	}
	SlaveFcts->Terminate = m_bdLibrary->get<fmi2Status(fmi2Component c)>(sFunction);
	sFunction = FCT_FMIRESET;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIRESET ".";
		return false;
	}
	SlaveFcts->Reset = m_bdLibrary->get<fmi2Status(fmi2Component c)>(sFunction);
	sFunction = FCT_FMIFREEINSTANCE;
	if (HasFunction(sFunction, false) == false) {
		m_sError = "Failed to load function " FCT_FMIFREEINSTANCE ".";
		return false;
	}
	SlaveFcts->FreeInstance = m_bdLibrary->get<void(fmi2Component c)>(sFunction);
	//
	m_eStatus = SLAVE_STATUS_FCTS_MAPPED;
	return true;
}

bool CSlave2_0::Instantiate()
{
	if ((m_eStatus != SLAVE_STATUS_FCTS_MAPPED) && (m_eStatus != SLAVE_STATUS_RELEASED)) {
		m_sError = "Failed to instantiate slave model. Please load the library first.";
		m_eErrorCode = SLAVE_ERROR_INSTANTIATE_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	try {
		if (m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetNode("CoSimulation") == 0) {
			m_eErrorCode = SLAVE_ERROR_NOTFORCOSIMULATION;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
		}
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E5;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	std::string sModelIdentifier;
	try {
		sModelIdentifier = m_cFMU->GetRoot()->GetNode("fmiModelDescription")
			->GetNode("CoSimulation")->GetAttribute("modelIdentifier");
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E6;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	std::string sGUID;
	try {
		sGUID = m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetAttribute("guid");
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E7;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Get resources path
	std::string sResourcesLoc = boost::str(boost::format(RESOURCES_LOCATION) % m_cModel->GetDir());
	// Instantiate
	*(void **)&(CSlaveFunctions2_0::m_fCallbacks.componentEnvironment) = this;
	if (m_cModel->UseLogger()) {
		SlaveFcts->m_bUseLogger = true;
	}
	m_cComponent = (*SlaveFcts->Instantiate)(sModelIdentifier.c_str(), fmi2CoSimulation,
		sGUID.c_str(), sResourcesLoc.c_str(), &CSlaveFunctions2_0::m_fCallbacks,
		(m_bVisible ? fmi2True : fmi2False), fmi2True);
	// Check the component validity
	if (m_cComponent == NULL) {
		m_sError = "Failed to instantiate the model.";
		m_eErrorCode = SLAVE_ERROR_NO_COMPONENT;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Get log categories
	isl::CXMLNode * cxLogCats = 0;
	try {
		cxLogCats = m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetNode("LogCategories");
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E8;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	m_nCategories = 0;
	if (cxLogCats != 0) {
		m_nCategories = cxLogCats->CountNodes();
	}
	if (m_nCategories > 0) {
		m_lsCategories = (char** )calloc(m_nCategories, sizeof(char *));
		for (int i = 0; i < m_nCategories; i++) {
			std::string sName = cxLogCats->GetNode(i)->GetAttribute("name");
			m_lsCategories[i] = (char *)calloc(sName.size() + 1, sizeof(char));
			isl::CString::Copy(m_lsCategories[i], sName.size() + 1, sName.c_str());
		}
		fmi2Status eStatus = SlaveFcts->SetDebugLogging(m_cComponent, fmi2True, m_nCategories, m_lsCategories);
		// Free the memory
		for (int i = 0; i < m_nCategories; i++) {
			free(m_lsCategories[i]);
			m_lsCategories[i] = NULL;
		}
		free(m_lsCategories);
		m_lsCategories = NULL;
		if (eStatus > fmi2Warning) {
			m_sError = "Failed to set debug logging.";
			m_eErrorCode = SLAVE_ERROR_FAILED_DEBUGLOGGING;
		}
	}
	//
	m_eStatus = SLAVE_STATUS_INSTANTIATED;
	return true;
}

bool CSlave2_0::Initialize()
{
	if ((m_eStatus != SLAVE_STATUS_INSTANTIATED) && (m_eStatus != SLAVE_STATUS_TERMINATED)) {
		m_sError = "Failed to initialize model. Model not instantiated.";
		m_eErrorCode = SLAVE_ERROR_ENTERINITIALIZATION_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Should initialize all variables with the field 'start'
	fmi2Boolean bToleranceDefined = fmi2False;
	double dTolerance = 0.0;
	double dStartTime = 0.0;
	fmi2Boolean bStopTimeDefined = fmi2False;
	double dStopTime = 0.0;
	isl::CXMLNode * cxDefExp = 0;
	try {
		cxDefExp = m_cFMU->GetRoot()->GetNode("fmiModelDescription")->GetNode("DefaultExperiment");
	}
	catch (...) {
		m_eErrorCode = SLAVE_ERROR_FMUXML_E9;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (cxDefExp != 0) {
		std::string sStartTime = cxDefExp->GetAttribute("startTime");
		std::string sStopTime = cxDefExp->GetAttribute("stopTime");
		std::string sTolerance = cxDefExp->GetAttribute("tolerance");
		if (sTolerance.empty() == false) {
			bToleranceDefined = fmi2True;
			dTolerance = std::stod(sTolerance); // could be replaced by boost::lexical_cast
		}
		if (sStartTime.empty() == false) {
			dStartTime = std::stod(sStartTime); // could be replaced by boost::lexical_cast
		}
		if (sStopTime.empty() == false) {
			dStopTime = std::stod(sStopTime); // could be replaced by boost::lexical_cast
			bStopTimeDefined = fmi2True;
		}
	}
	// Start time fixed in priority with the black box
	if (m_cBlackBox->GetStartTime() > dStartTime) {
		dStartTime = m_cBlackBox->GetStartTime();
	}
	if (m_cBlackBox->GetEndTime() > 0.0) {
		dStopTime = m_cBlackBox->GetEndTime();
		bStopTimeDefined = fmi2True;
	}
	// Setup the experiment
	fmi2Status eStatus = SlaveFcts->SetupExperiment(m_cComponent, bToleranceDefined, dTolerance,
		dStartTime, bStopTimeDefined, dStopTime);
	if (eStatus > fmi2Warning) {
		m_sError = "Failed to initialize model: error on setup experiment.";
		m_eErrorCode = SLAVE_ERROR_FAILED_SETUPEXPERIMENT;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Enter initialization mode
	eStatus = SlaveFcts->EnterInitializationMode(m_cComponent);
	if (eStatus > fmi2Warning) {
		m_sError = "Failed to initialize model: error on enter initialization mode.";
		m_eErrorCode = SLAVE_ERROR_FAILED_ENTERINIT;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	m_eStatus = SLAVE_STATUS_ENTERINITIALIZATION;
	return true;
}

bool CSlave2_0::EndInitialize()
{
	if (m_eStatus != SLAVE_STATUS_ENTERINITIALIZATION) {
		m_sError = "Failed to end the initialization. Model not initialized.";
		m_eErrorCode = SLAVE_ERROR_INITIALIZED_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	// Should be called after initializing inputs (initial = 'exact' or 'approx') with fmi2SetXXX functions
	if (SlaveFcts->ExitInitializationMode(m_cComponent) > fmi2Warning) {
		m_sError = "Failed to initialize model: error on exit initialization mode.";
		m_eErrorCode = SLAVE_ERROR_FAILED_EXITINIT;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	m_eStatus = SLAVE_STATUS_INITIALIZED;
	return true;
}

bool CSlave2_0::DoStep(double dTime, double dTimeStep, bool noSetState)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)) {
		m_eErrorCode = SLAVE_ERROR_DOSTEP_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	fmi2Status eStatus = SlaveFcts->DoStep(m_cComponent, dTime, dTimeStep, (noSetState ? fmi2True : fmi2False));
	switch (eStatus) {
		case fmi2OK:
			break;
		case fmi2Pending:
			if (Lock() == false) {
				m_eErrorCode = SLAVE_ERROR_DOSTEP_TIMEOUT_REACHED;
				m_eStatus = SLAVE_STATUS_ERROR;
				return false;
			}
			break;
		case fmi2Discard:
			{
				fmi2Boolean bVal = fmi2False;
				SlaveFcts->GetBooleanStatus(m_cComponent, fmi2Terminated, &bVal);
				if (bVal == fmi2True) {
					m_eErrorCode = SLAVE_ERROR_STOP_REQUIRED;
					m_sError = "Stop required by the FMI model.";
					m_eStatus = SLAVE_STATUS_ERROR;
					break;
				}
			}
		case fmi2Error:
		default:
			m_sError = "Failed to complete simulation of the model.";
			m_eErrorCode = SLAVE_ERROR_SIMULATION_FAILED;
			m_eStatus = SLAVE_STATUS_ERROR;
			return false;
	}
	//
	m_eStatus = SLAVE_STATUS_RUNNING;
	return true;
}

bool CSlave2_0::SetReal(unsigned int uRef, double * dVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)
			&& (m_eStatus != SLAVE_STATUS_INSTANTIATED) && (m_eStatus != SLAVE_STATUS_ENTERINITIALIZATION)) {
		m_sError = "Failed to set real. Model not running or not initialized.";
		m_eErrorCode = SLAVE_ERROR_SETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (SlaveFcts->SetReal(m_cComponent, &uRef, 1, dVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_SETREAL_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::GetReal(unsigned int uRef, double * dVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)) {
		m_sError = "Failed to get real. Model not running.";
		m_eErrorCode = SLAVE_ERROR_GETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (SlaveFcts->GetReal(m_cComponent, &uRef, 1, dVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_GETREAL_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::SetInteger(unsigned int uRef, int * nVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)
			&& (m_eStatus != SLAVE_STATUS_INSTANTIATED) && (m_eStatus != SLAVE_STATUS_ENTERINITIALIZATION)) {
		m_sError = "Failed to set integer. Model not running or not initialized.";
		m_eErrorCode = SLAVE_ERROR_SETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (SlaveFcts->SetInteger(m_cComponent, &uRef, 1, nVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_SETINTEGER_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::GetInteger(unsigned int uRef, int * nVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)) {
		m_sError = "Failed to get integer. Model not running.";
		m_eErrorCode = SLAVE_ERROR_GETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (SlaveFcts->GetInteger(m_cComponent, &uRef, 1, nVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_GETINTEGER_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::SetBoolean(unsigned int uRef, bool * bVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)
			&& (m_eStatus != SLAVE_STATUS_INSTANTIATED) && (m_eStatus != SLAVE_STATUS_ENTERINITIALIZATION)) {
		m_sError = "Failed to set boolean. Model not running or not initialized.";
		m_eErrorCode = SLAVE_ERROR_SETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	fmi2Boolean fbVal = (bVal ? fmi2True : fmi2False);
	if (SlaveFcts->SetBoolean(m_cComponent, &uRef, 1, &fbVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_SETBOOLEAN_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::GetBoolean(unsigned int uRef, bool * bVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)) {
		m_sError = "Failed to get boolean. Model not running.";
		m_eErrorCode = SLAVE_ERROR_GETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	fmi2Boolean fbVal = fmi2False;
	if (SlaveFcts->GetBoolean(m_cComponent, &uRef, 1, &fbVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_GETBOOLEAN_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	*bVal = (fbVal == fmi2True ? true : false);
	//
	return true;
}

bool CSlave2_0::SetString(unsigned int uRef, char * sVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)
			&& (m_eStatus != SLAVE_STATUS_INSTANTIATED) && (m_eStatus != SLAVE_STATUS_ENTERINITIALIZATION)) {
		m_sError = "Failed to set string. Model not running or not initialized.";
		m_eErrorCode = SLAVE_ERROR_SETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	fmi2String fsVal = (fmi2String)sVal;
	if (SlaveFcts->SetString(m_cComponent, &uRef, 1, &fsVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_SETSTRING_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::GetString(unsigned int uRef, char * sVal)
{
	if ((m_eStatus != SLAVE_STATUS_INITIALIZED) && (m_eStatus != SLAVE_STATUS_RUNNING)) {
		m_sError = "Failed to get string. Model not running.";
		m_eErrorCode = SLAVE_ERROR_GETDATA_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	if (SlaveFcts->GetString(m_cComponent, &uRef, 1, (fmi2String *)sVal) > fmi2Warning) {
		m_eErrorCode = SLAVE_ERROR_GETSTRING_FAILED;
		m_eStatus = SLAVE_STATUS_ERROR;
		return false;
	}
	//
	return true;
}

bool CSlave2_0::Terminate()
{
	if ((m_eStatus == SLAVE_STATUS_INITIALIZED) || (m_eStatus == SLAVE_STATUS_RUNNING)) {
		SlaveFcts->Terminate(m_cComponent);
	}
	m_eStatus = SLAVE_STATUS_TERMINATED;
	//
	return true;
}

bool CSlave2_0::Reset()
{
	if ((m_eStatus == SLAVE_STATUS_INITIALIZED) || (m_eStatus == SLAVE_STATUS_RUNNING)) {
		SlaveFcts->Reset(m_cComponent);
	}
	m_eStatus = SLAVE_STATUS_TERMINATED;
	//
	return true;
}

bool CSlave2_0::Free()
{
	if ((m_eStatus == SLAVE_STATUS_TERMINATED) ||
		(m_eStatus == SLAVE_STATUS_INSTANTIATED) ||
		(m_eStatus == SLAVE_STATUS_INITIALIZED) ||
		(m_eStatus == SLAVE_STATUS_ENTERINITIALIZATION)) {
		if (m_cComponent  != 0)
		{
			SlaveFcts->FreeInstance(m_cComponent);
			m_cComponent = 0;
		}
	}
	m_eStatus = SLAVE_STATUS_RELEASED;
	//
	return true;
}
