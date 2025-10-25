/* 
 *     Name: fmi2Functions.cpp
 *
 *     Description: FMI 2.0 functions.
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

#include <memory>
#include <iostream>
#include <sstream>
#include <cstring>

#ifdef DEBUG
#define FMI_ASSERT(p)	if (!(p)) \
	{ std::cerr << "Assertion failure\nCHECK: " << #p << "\nFILE:  " << myFilename(__FILE__) << "\nLINE:  " << __LINE__ << '\n'; \
	  return fmi2Error; }
#else // No DEBUG
#define FMI_ASSERT(p) (void)0;
#endif //  DEBUG

#ifdef _WIN32
#if _WIN32_WINNT < 0x0501
#define _WIN32_WINNT 0x0501
#endif

#include <windows.h>
#endif // _WIN32

#include "v2.0/fmi2Functions.h"
#include "islfmuslave.h"


/*
 *     FMI Functions definition
 */


const char* fmi2GetTypesPlatform() {
	// returns platform type = "default"
	return fmi2TypesPlatform;
}


const char* fmi2GetVersion() {
	// returns fmi version
	return "2.0";
}


// Enables/disables debug logs
fmi2Status fmi2SetDebugLogging(void* component, fmi2Boolean loggingOn,
		size_t nCategories, const char* const categories[]) {
	ISLFMUSlave * modelInstance = static_cast<ISLFMUSlave* >(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2OK, "logAll",
		std::string("fmi2SetDebugLogging: logging switched ") + (loggingOn ? "on." : "off."));
	modelInstance->m_loggingOn = (loggingOn == fmi2True);
	if (modelInstance->m_loggingOn) {
		modelInstance->m_loggingCategories.clear();
		for (size_t i=0; i<nCategories; ++i)
			modelInstance->m_loggingCategories.push_back(std::string(categories[i]));
	}
	return fmi2OK;
}

// Create the component instance
void* fmi2Instantiate(fmi2String instanceName, fmi2Type fmuType, fmi2String guid,
					  fmi2String fmuResourceLocation,
					  const fmi2CallbackFunctions* functions,
					  fmi2Boolean, fmi2Boolean loggingOn)
{
	if (functions == NULL)
		return NULL;

	if (functions->logger == NULL)
		return NULL;

	std::string instanceNameString = instanceName;
	if (instanceNameString.empty()) {
		if (loggingOn)
			functions->logger(functions->componentEnvironment, instanceName, fmi2Error,
				"logStatusError", "fmi2Instantiate: Missing instance name.");
		return NULL;
	}

	// instantiate the FMU component
	ISLFMUSlave * component = ISLFMUSlave::Create();
	component->m_callbackFunctions = functions;
	component->m_instanceName = instanceName;
	component->m_GUID = guid;
	component->m_isCosimulation = (fmuType == fmi2CoSimulation);
	component->m_resourceLocation = fmuResourceLocation;
	component->m_loggingOn = loggingOn;

	return component;
}


// Free the allocated component instance
void fmi2FreeInstance(void* component) {
	ISLFMUSlave * modelInstance = static_cast<ISLFMUSlave* >(component);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2FreeInstance: Model instance deleted.");
	delete modelInstance;
}


// Overrides simulation settings
fmi2Status fmi2SetupExperiment(void* component, int, double, double, int, double)
{
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2SetupExperiment: Call of setup experiment.");
	return fmi2OK;
}


// Initialize the variables (initial = "exact" or "approx")
fmi2Status fmi2EnterInitializationMode(void* component) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2EnterInitializationMode: Go into initialization mode.");
	modelInstance->m_initializationMode = true;

	try {
		modelInstance->Init();
		// States management should be initialized here
		return fmi2OK;
	}
	catch (std::exception & ex) {
		std::string err = ex.what();
		err += "\nModel initialization failed.";
		modelInstance->Logger(fmi2Error, "logStatusError", err);
		return fmi2Error;
	}
}


// Switch off all initialization equations
fmi2Status fmi2ExitInitializationMode(void* component) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	try {
		modelInstance->EndInit();
	}
	catch (std::exception & ex) {
		std::string err = ex.what();
		err += "\nModel end initialization failed.";
		modelInstance->Logger(fmi2Error, "logStatusError", err);
		return fmi2Error;
	}
	modelInstance->Logger(fmi2OK, "logAll", "fmi2ExitInitializationMode: Go out from initialization mode.");
	modelInstance->m_initializationMode = false;
	return fmi2OK;
}


// Terminate/reset functions
fmi2Status fmi2Terminate(void* component) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Terminate();
	modelInstance->Logger(fmi2OK, "logAll", "fmi2Terminate: Terminate model.");
	return fmi2OK;
}


fmi2Status fmi2Reset(void* component) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2Reset: Reset the whole model to default. Not implemented yet.");
	return fmi2OK;
}


// Data access functions
fmi2Status fmi2GetReal(void* component, const fmi2ValueReference vr[], size_t nvr, fmi2Real value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->GetReal(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2GetReal()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2GetInteger(void* component, const fmi2ValueReference vr[], size_t nvr, fmi2Integer value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->GetInt(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2GetInteger()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2GetBoolean(void* component, const fmi2ValueReference vr[], size_t nvr, fmi2Boolean value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			bool val;
			modelInstance->GetBool(vr[i], val);
			value[i] = val;
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2GetBoolean()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2GetString(void* component, const fmi2ValueReference vr[], size_t nvr, fmi2String value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->GetString(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2GetString()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2SetReal (void* component, const fmi2ValueReference vr[], size_t nvr, const fmi2Real value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->SetReal(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2SetReal()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2SetInteger(void* component, const fmi2ValueReference vr[], size_t nvr, const fmi2Integer value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->SetInt(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2SetInteger()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2SetBoolean(void* component, const fmi2ValueReference vr[], size_t nvr, const fmi2Boolean value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->SetBool(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2SetBoolean()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


fmi2Status fmi2SetString(void* component, const fmi2ValueReference vr[], size_t nvr, const fmi2String value[]) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	for (size_t i=0; i<nvr; ++i) {
		try {
			modelInstance->SetString(vr[i], value[i]);
		}
		catch (std::exception & ex) {
			std::string err = ex.what();
			err += "\nError in fmi2SetString()";
			modelInstance->Logger(fmi2Error, "logStatusError", err);
			return fmi2Error;
		}
	}
	return fmi2OK;
}


// States not managed yet
fmi2Status fmi2GetFMUstate(void* component, fmi2FMUstate* FMUstate) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2GetFMUstate: Not implemented yet.");
	return fmi2Warning;
}


fmi2Status fmi2SetFMUstate(void* component, fmi2FMUstate FMUstate) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2SetFMUstate: Not implemented yet.");
	return fmi2Warning;
}


fmi2Status fmi2FreeFMUstate(void* component, fmi2FMUstate* FMUstate) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2FreeFMUstate: Not implemented yet.");
	return fmi2Warning;
}


fmi2Status fmi2SerializedFMUstateSize(fmi2Component component, fmi2FMUstate FMUstate, size_t* s) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2SerializedFMUstateSize: Not implemented yet.");
	return fmi2Warning;
}


fmi2Status fmi2SerializeFMUstate(fmi2Component component, fmi2FMUstate FMUstate, fmi2Byte serializedState[], size_t /*s*/) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2SerializeFMUstate: Not implemented yet.");
	return fmi2Warning;
}


fmi2Status fmi2DeSerializeFMUstate(void* component, const char serializedState[], size_t s, fmi2FMUstate*  FMUstate) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2DeSerializeFMUstate: Not implemented yet.");
	return fmi2Warning;
}



// Derivatives
fmi2Status fmi2GetDirectionalDerivative(void* component, const unsigned int[], size_t,
																const unsigned int[], size_t,
																const double[], double[])
{
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2GetDirectionalDerivative: Not implemented yet.");
	return fmi2Warning;
}


fmi2Status fmi2SetRealInputDerivatives(void* component,	const fmi2ValueReference vr[], size_t nvr,
										const fmi2Integer order[], const fmi2Real value[])
{
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2SetRealInputDerivatives: Not implemented yet.");
	return fmi2OK;
}


fmi2Status fmi2GetRealOutputDerivatives(void* component, const fmi2ValueReference vr[], size_t nvr,
										const fmi2Integer order[], fmi2Real value[])
{
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);
	FMI_ASSERT(modelInstance != NULL);
	modelInstance->Logger(fmi2Warning, "logStatusWarning", "fmi2GetRealOutputDerivatives: Not implemented yet.");
	return fmi2OK;
}


// Co-simulation step
fmi2Status fmi2DoStep(void* component, double currentCommunicationPoint, double communicationStepSize,
					  int noSetFMUStatePriorToCurrentPoint)
{
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(modelInstance->m_isCosimulation);

	if (noSetFMUStatePriorToCurrentPoint == fmi2True) {
		// Check what needs to be done
	}

	try {
		modelInstance->IntegrateTo(currentCommunicationPoint, communicationStepSize);
	}
	catch (std::exception & ex) {
		std::string err = ex.what();
		err += "\nfmi2DoStep: Exception while integrating model";
		modelInstance->Logger(fmi2Error, "logStatusError", err);
		return fmi2Error;
	}
	return fmi2OK;
}


fmi2Status fmi2CancelStep(void* component) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(modelInstance->m_isCosimulation);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2CancelStep: cancel current step.");
	return fmi2OK;
}


fmi2Status fmi2GetStatus(void* component, const fmi2StatusKind s, fmi2Status* value) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(modelInstance->m_isCosimulation);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2GetStatus: get current status.");
	return fmi2OK;
}


fmi2Status fmi2GetRealStatus(void* component, const fmi2StatusKind s, fmi2Real* value) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(!modelInstance->m_modelExchange);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2GetRealStatus: get real status.");
	return fmi2OK;
}


fmi2Status fmi2GetIntegerStatus(void* component, const fmi2StatusKind s, fmi2Integer* value) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(!modelInstance->m_modelExchange);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2GetIntegerStatus: get integer status.");
	return fmi2OK;
}


fmi2Status fmi2GetBooleanStatus(void* component, const fmi2StatusKind s, fmi2Boolean* value) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(!modelInstance->m_modelExchange);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2GetBooleanStatus: get boolean status.");
	return fmi2OK;
}


fmi2Status fmi2GetStringStatus(void* component, const fmi2StatusKind s, fmi2String* value) {
	ISLFMUSlave* modelInstance = static_cast<ISLFMUSlave*>(component);

	FMI_ASSERT(modelInstance != NULL);
	FMI_ASSERT(!modelInstance->m_modelExchange);
	modelInstance->Logger(fmi2OK, "logAll", "fmi2GetStringStatus: get string status.");
	return fmi2OK;
}
