/*
 *     Name: isl_variable.h
 *
 *     Description: ISL API Variable class.
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
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>

#include <isl_misc.h>
#include <isl_log.h>
#include <isl_api.h>

#include "isl_errorcodes.h"


/*
 *     Classes definition
 */

isl::CDataType::CDataType(tType eType, int nSize)
{
	m_eType = eType;
	m_nSize = nSize;
	if (m_nSize <= 0) {
		AppLogWarning(ISLDATATYPE_POSITIVE_SIZE, "Size must be a number >= 1. Size set to 1.");
		m_nSize = 1;
	}
	if (m_eType == TP_STRUCTURE && m_nSize > 1) {
		AppLogWarning(ISLDATATYPE_STRUCTURE_NO_ARRAY, "Structured types don't support arrays. Size set to 1.");
		m_nSize = 1;
	}
	m_nSizeOf = GetSizeOf(m_eType);
	m_nSizeInBytes = m_nSize * m_nSizeOf;

	m_bInitialSet = false;
	m_pInitial = 0;
	m_pValue = 0;

	m_bAllocated = false;

	m_pSubInitials = 0;
	m_pSubValues = 0;
}

isl::CDataType::~CDataType()
{
	Free();
}

bool isl::CDataType::AddSubType(const std::string & sField, tType eType, int nSize)
{
	if (m_bAllocated) {
		AppLogError(ISLDATATYPE_ALREADY_ALLOCATED, "Cannot add a subtype after memory allocation.");
		return false;
	}
	if (sField.empty()) {
		AppLogError(ISLDATATYPE_NOFIELD, "The subtype name (field) cannot be empty.");
		return false;
	}
	if (IsValidType(eType, true)) {
		AppLogError(ISLDATATYPE_NOTVALIDSUBTYPE, "The subtype %d is not a valid one.", eType);
		return false;
	}
	bool bFound = false;
	try {
		CDataType * cUnusedVar = m_mSubTypes.at(sField);
	}
	catch (...) {
		bFound = true;
	}
	if (bFound) {
		AppLogError(ISLDATATYPE_FIELD_ALREADYEXISTS, "The subtype name '%s' already exists.", sField.c_str());
		return false;
	}
	CDataType * cType = new CDataType(eType, nSize);
	m_lSubTypes.push_back(cType);
	m_mSubTypes[sField] = cType;
	return true;
}

isl::CDataType * isl::CDataType::GetSubType(const std::string & sField)
{
	try {
		return m_mSubTypes.at(sField);
	}
	catch (...) {
		return 0;
	}
	return 0;
}

isl::CDataType * isl::CDataType::GetSubType(int i)
{
	if ((i < 0) || ((size_t )i >= m_lSubTypes.size())) {
		return 0;
	}
	return m_lSubTypes[i];
}

int isl::CDataType::GetNbOfSubTypes()
{
	return (int )(m_lSubTypes.size());
}

isl::CDataType::tType isl::CDataType::GetId()
{
	return m_eType;
}

std::string isl::CDataType::GetIdAsStr()
{
	std::string sId;
	switch (m_eType) {
		case TP_BOOLEAN:
			sId = "Boolean";
			break;
		case TP_INTEGER:
			sId = "Integer";
			break;
		case TP_REAL:
			sId = "Real";
			break;
		case TP_STRING:
			sId = "String";
			break;
		case TP_STRUCTURE:
			sId = "Structure";
			break;
		default:
			break;
	}
	return sId;
}

void isl::CDataType::SetLabel(const std::string & sVal)
{
	m_sLabel = sVal;
}

std::string isl::CDataType::GetLabel()
{
	return m_sLabel;
}

int isl::CDataType::GetSize()
{
	return m_nSize;
}

int isl::CDataType::GetSizeOf()
{
	return m_nSizeOf;
}

int isl::CDataType::GetSizeInBytes()
{
	return m_nSizeInBytes;
}

bool isl::CDataType::IsInitialSet()
{
	return m_bInitialSet;
}

void * isl::CDataType::GetInitial()
{
	return m_pInitial;
}

std::string isl::CDataType::GetInitialAsStr()
{
	std::string sInit;
	char * pVal = (char *)m_pInitial;
	for (int i = 0; i < m_nSize; i++) {
		switch (m_eType) {
			case TP_REAL:
				sInit += boost::str(boost::format("%1%") % *(double *)pVal);
				break;
			case TP_INTEGER:
				sInit += boost::str(boost::format("%1%") % *(int *)pVal);
				break;
			case TP_BOOLEAN:
				if (*(bool *)pVal) {
					sInit += "true";
				}
				else {
					sInit += "false";
				}
				break;
			case TP_STRING:
				sInit += boost::str(boost::format("%1%") % *(char *)pVal);
				break;
			case TP_STRUCTURE:
				AppLogWarning(ISLDATATYPE_NOINITIAL_TOSTR,
					"Initial values for structured types cannot be formatted in a string.");
			default:
				return sInit;
		}
		if (i + 1 != m_nSize) {
			sInit += ",";
		}
		pVal = pVal + m_nSizeOf;
	}
	return sInit;
}

void * isl::CDataType::GetValue()
{
	return m_pValue;
}

bool isl::CDataType::SetInitial(const std::string & sVal)
{
	if (sVal.empty()) {
		AppLogWarning(ISLDATATYPE_SETINITIAL_NOVAL,
			"No value can be used for initialization.");
		return false;
	}
	// If the type is structured, the allocation can only be done subtype per subtype
	if (m_eType == TP_STRUCTURE) {
		AppLogError(ISLDATATYPE_SETINITIAL_STRUCTURE,
			"Initial values must be set on each field on the structured type.");
		return false;
	}
	// No memory allocation is done... only a copy of the elements
	if (m_bAllocated == false) {
		AppLogError(ISLDATATYPE_SETINITIAL_NOTALLOCATED,
			"Initial values cannot be set on non memory allocated types.");
		return false;
	}
	// String shall be set like "val1,val2,val3"
	boost::char_separator<char> cSep(",");
	boost::tokenizer<boost::char_separator<char>> cTokens(sVal, cSep);
	size_t nNbElts = std::distance(cTokens.begin(), cTokens.end());
	if (nNbElts <= 0) {
		AppLogError(ISLDATATYPE_SETINITIAL_NOELEMENT,
			"Cannot extract initial values from the string '%s'.", sVal.c_str());
		return false;
	}
	boost::tokenizer<boost::char_separator<char>>::iterator iElt;
	size_t i = 0;
	for (iElt = cTokens.begin(); iElt != cTokens.end(); iElt++) {
		if (i < m_nSize) {
			switch (m_eType) {
				case TP_REAL:
					((double *)m_pInitial)[i] = CString::GetDouble(*iElt);
					break;
				case TP_INTEGER:
					((int *)m_pInitial)[i] = CString::GetInteger(*iElt);
					break;
				case TP_BOOLEAN:
					((bool *)m_pInitial)[i] = false;
					if (*iElt == "true") {
						((bool *)m_pInitial)[i] = true;
					}
					break;
				case TP_STRING:
					((char *)m_pInitial)[i] = (*iElt)[0];
					break;
				case TP_STRUCTURE:
					AppLogWarning(ISLDATATYPE_NOINITIAL_FROMSTR,
						"Initial values for structured types cannot be set with a string.");
				default:
					break;
			}
		}
	}
	m_bInitialSet = true;
	return true;
}

bool isl::CDataType::SetInitial(void * pVal)
{
	// If the type is structured, the allocation can only be done subtype per subtype
	if (m_eType == TP_STRUCTURE) {
		AppLogError(ISLDATATYPE_SETINITIAL_STRUCTURE,
			"Initial values must be set on each field on the structured type.");
		return false;
	}
	// No memory allocation is done... only a copy of the elements
	if (m_bAllocated == false) {
		AppLogError(ISLDATATYPE_SETINITIAL_NOTALLOCATED,
			"Initial values cannot be set on non memory allocated types.");
		return false;
	}
	// We suppose the types of pVal and of m_pInitial match together
	try {
		memcpy(m_pInitial, pVal, m_nSizeInBytes);
	}
	catch(...) {
		AppLogError(ISLDATATYPE_SETINITIAL_FAILED,
			"Unexpected error when setting the initial values.");
		return false;
	}
	m_bInitialSet = true;
	return true;
}

bool isl::CDataType::SetValue(void * pVal)
{
	// If the type is structured, the allocation can only be done subtype per subtype
	if (m_eType == TP_STRUCTURE) {
		AppLogError(ISLDATATYPE_SETVALUE_STRUCTURE,
			"Values must be set on each field on the structured type.");
		return false;
	}
	// No memory allocation is done... only a copy of the elements
	if (m_bAllocated == false) {
		AppLogError(ISLDATATYPE_SETVALUE_NOTALLOCATED,
			"Values cannot be set on non memory allocated types.");
		return false;
	}
	// We suppose the types of pVal and of m_pInitial match together
	try {
		memcpy(m_pValue, pVal, m_nSizeInBytes);
	}
	catch (...) {
		AppLogError(ISLDATATYPE_SETVALUE_FAILED,
			"Unexpected error when setting the values.");
		return false;
	}
	return true;
}

void isl::CDataType::Allocate()
{
	if (m_bAllocated) {
		return;
	}
	if (m_pInitial || m_pValue) {
		m_bAllocated = true;
		return;
	}
	switch (m_eType) {
		case TP_REAL:
		case TP_INTEGER:
		case TP_BOOLEAN:
		case TP_STRING:
			m_pInitial = calloc(m_nSize, m_nSizeOf);
			m_pValue = calloc(m_nSize, m_nSizeOf);
			break;
		case TP_STRUCTURE:
			if (m_lSubTypes.empty() == false) {
				m_pSubInitials = (void **)calloc(m_lSubTypes.size(), sizeof(void*));
				m_pSubValues = (void **)calloc(m_lSubTypes.size(), sizeof(void*));
				for (size_t i = 0; i < m_lSubTypes.size(); i++) {
					m_lSubTypes[i]->Allocate();
					m_pSubInitials[i] = m_lSubTypes[i]->GetInitial();
					m_pSubValues[i] = m_lSubTypes[i]->GetValue();
				}
				m_pInitial = m_pSubInitials[0];
				m_pValue = m_pSubValues[0];
			}
			break;
		default:
			// Not allocated
			return;
	}
	m_bAllocated = true;
}

void isl::CDataType::Free()
{
	if (m_lSubTypes.empty() == false) {
		for (size_t i = 0; i < m_lSubTypes.size(); i++) {
			m_lSubTypes[i]->Free();
			delete m_lSubTypes[i];
		}
	}
	m_lSubTypes.clear();
	m_mSubTypes.clear();
	if (m_bAllocated) {
		try {
			if (m_eType != TP_STRUCTURE) {
				if (m_pInitial) {
					free(m_pInitial);
				}
				if (m_pValue) {
					free(m_pValue);
				}
			}
			if (m_pSubInitials) {
				free(m_pSubInitials);
			}
			if (m_pSubValues) {
				free(m_pSubValues);
			}
		}
		catch (...) {
			AppLogError(ISLDATATYPE_FREE_ERROR, "Failed to free the memory.");
		}
	}
	m_bInitialSet = false;
	m_pInitial = 0;
	m_pValue = 0;
	m_pSubInitials = 0;
	m_pSubValues = 0;
	m_bAllocated = false;
}

bool isl::CDataType::IsAllocated()
{
	return m_bAllocated;
}

bool isl::CDataType::IsValidType(tType eType, bool bIsSubType)
{
	switch (eType) {
	case TP_REAL:
	case TP_INTEGER:
	case TP_BOOLEAN:
	case TP_STRING:
		return true;
	case TP_STRUCTURE:
		if (bIsSubType == false) {
			return true;
		}
	default:
		break;
	}
	return false;
}

int isl::CDataType::GetSizeOf(tType eType)
{
	switch (eType) {
		case TP_REAL:
			return sizeof(double);
		case TP_INTEGER:
			return sizeof(int);
		case TP_BOOLEAN:
			return sizeof(bool);
		case TP_STRING:
			return sizeof(char);
		default:
			break;
	}
	return 0;
}

isl::CVariable::CVariable(CConnect * cParent, const std::string & sId)
{
	m_cParent = cParent;
	m_eState = ST_PARTIALLY_DEFINED;
	m_bChecked = false;
	m_sId = sId;
	m_cType = 0;
	m_eCausality = CS_UNKNOWN;
	m_dStepSize = -1.0; // <0 means no local step size has been defined
	m_bStore = false;
	m_nSyncTimeout = -1; // <0 means infinite wait
	if (m_cParent == 0) {
		m_eState = ST_NOT_DEFINED;
		AppLogError(ISLVARIABLE_NO_PARENT, "Variable %s must be linked to a connector.", sId.c_str());
	}
	if (m_sId.empty()) {
		m_eState = ST_NOT_DEFINED;
		AppLogError(ISLVARIABLE_EMPTY_ID, "The variable must have a valid identifier");
	}
	if (m_eState == ST_PARTIALLY_DEFINED) {
		if (m_cParent->IOExists(m_sId)) {
			m_eState = ST_NOT_DEFINED;
			AppLogError(ISLVARIABLE_VAR_EXISTS,
				"The variable %s already exists in the current connector.", m_sId.c_str());
		}
	}
}

isl::CVariable::~CVariable()
{
	if (m_cType) {
		delete m_cType;
	}
	m_cType = 0;
}

isl::CVariable::tState isl::CVariable::GetState()
{
	if (m_eState != ST_NOT_DEFINED) {
		if ((m_eCausality != CS_UNKNOWN) && (m_cType != 0)) {
			m_eState = ST_FULLY_DEFINED;
			if (m_sConnectId.empty() == false) {
				m_eState = ST_IS_CONNECTED;
			}
		}
	}
	return m_eState;
}

isl::CConnect * isl::CVariable::GetParent()
{
	return m_cParent;
}

std::string isl::CVariable::GetId()
{
	return m_sId;
}

void isl::CVariable::SetName(const std::string & sName)
{
	m_sName = sName;
}

std::string isl::CVariable::GetName()
{
	return m_sName;
}

void isl::CVariable::SetConnectId(const std::string & sId)
{
	m_sConnectId = sId;
}

std::string isl::CVariable::GetConnectId()
{
	return m_sConnectId;
}

void isl::CVariable::SetCausality(const std::string & sVal)
{
	if (sVal == "input") {
		m_eCausality = CS_INPUT;
	}
	else if (sVal == "output") {
		m_eCausality = CS_OUTPUT;
	}
}

void isl::CVariable::SetCausality(tCausality eCausality)
{
	m_eCausality = eCausality;
}

isl::CVariable::tCausality isl::CVariable::GetCausality()
{
	return m_eCausality;
}

std::string isl::CVariable::GetCausalityAsStr()
{
	std::string sCausality;
	switch (m_eCausality) {
		case CS_INPUT:
			sCausality = "input";
			break;
		case CS_OUTPUT:
			sCausality = "output";
			break;
		default:
			break;
	}
	return sCausality;
}

bool isl::CVariable::IsOutput()
{
	return m_eCausality == CS_OUTPUT;
}

bool isl::CVariable::IsInput()
{
	return m_eCausality == CS_INPUT;
}

isl::CDataType::tType isl::CVariable::GetTypeId()
{
	if (m_cType) {
		return m_cType->GetId();
	}
	return CDataType::TP_UNKNOWN;
}

isl::CDataType * isl::CVariable::GetType()
{
	return m_cType;
}

bool isl::CVariable::SetType(CDataType::tType eType, int nSize)
{
	if (m_cType) {
		AppLogError(ISLVARIABLE_TYPE_ALREADYSET,
			"Variable %s: a data type has been already defined (type id: %d).",
			m_sId.c_str(), m_cType->GetId());
		return false;
	}
	if (CDataType::IsValidType(eType) == false) {
		AppLogError(ISLVARIABLE_TYPE_NOTVALID,
			"Variable %s: the data type (%d) is not valid.", m_sId.c_str(), eType);
		return false;
	}
	m_cType = new CDataType(eType, nSize);
	return true;
}

void isl::CVariable::SetStepSize(double dStep)
{
	m_dStepSize = dStep;
}

void isl::CVariable::UseStore(bool bUse)
{
	m_bStore = bUse;
}

bool isl::CVariable::IsStoreUsed()
{
	return m_bStore;
}

void isl::CVariable::SetSyncTimeout(int nTimeout) // Timeout in seconds, but must be used in milliseconds
{
	m_nSyncTimeout = nTimeout * 1000;
}

int isl::CVariable::GetSyncTimeout()
{
	return m_nSyncTimeout;
}

bool isl::CVariable::IsChecked()
{
	return m_bChecked;
}

bool isl::CVariable::Check()
{
	if (m_bChecked) {
		return true;
	}
	if (m_sId.empty()) {
		if (m_sName.empty()) {
			AppLogError(ISLVARIABLE_CHECK_NOIDNONAME, "The variable has no identifier and no name defined.");
			return false;
		}
		else {
			AppLogWarning(ISLVARIABLE_CHECK_NOID, "The variable with the name '%s' has no identifier defined."
				" The name will be used as the identifier", m_sName.c_str());
			m_sId = m_sName;
		}
	}
	if (m_sName.empty()) {
		AppLogWarning(ISLVARIABLE_CHECK_NONAME, "Variable '%s': no name is defined.", m_sId.c_str());
	}
	if (m_eCausality == CS_UNKNOWN) {
		AppLogError(ISLVARIABLE_CHECK_NOCAUSALITY, "Variable '%s': no causality is defined.", m_sId.c_str());
		return false;
	}
	if (m_cType == 0) {
		AppLogError(ISLVARIABLE_CHECK_NODATATYPE, "Variable '%s': no data type is defined.", m_sId.c_str());
		return false;
	}
	if (m_cType->IsValidType(m_cType->GetId()) == false) {
		AppLogError(ISLVARIABLE_CHECK_INVALIDTYPE, "Variable '%s': not a valid data type (%d).",
			m_sId.c_str(), GetTypeId());
		return false;
	}
	// TODO: Check a structured type
	// Not useful to check the size as it is redefined if a wrong size is set.
	// Must check if the memory is already allocated
	if (m_cType->IsAllocated() == false) {
		AppLogWarning(ISLVARIABLE_CHECK_ALLOCATION,
			"Variable '%s': Memory not yet allocated. Allocating the memory now.", m_sId.c_str());
		m_cType->Allocate();
	}
	// If a step size is set, it should be consistent with Start and End times
	if (m_dStepSize == 0.0) {
		AppLogWarning(ISLVARIABLE_CHECK_ZEROSTEPSIZE,
			"Variable '%s': Step size cannot be set to 0. Step size will be ignored.", m_sId.c_str());
		m_dStepSize = -1.0;
	}
	if (m_dStepSize > 0.0) {
		if (m_dStepSize >= m_cParent->GetEndTime() - m_cParent->GetStartTime()) {
			AppLogWarning(ISLVARIABLE_CHECK_WRONGSTEPSIZE,
				"Variable '%s': The step size (%gs) cannot be greater than the difference between the end (%gs) and start (%gs) times."
				" Step size will be ignored.", m_sId.c_str(), m_dStepSize, m_cParent->GetEndTime(), m_cParent->GetStartTime());
			m_dStepSize = -1.0;
		}
	}
	if (m_nSyncTimeout == 0) {
		AppLogWarning(ISLVARIABLE_CHECK_ZEROSYNCTIMEOUT, "Variable '%s': Setting the synchronisation timeout value to 0"
			" is like setting the flag bWait to false in SetData and GetData methods.", m_sId.c_str());
	}
	if (m_sConnectId.empty()) {
		AppLogWarning(ISLVARIABLE_CHECK_NOTCONNECTED, "Variable '%s': will no be connected to ISL.",
			m_sId.c_str());
	}
	// Initialize data variables
	InitOnCheck();
	// TODO: check the compute settings
	AppLogInfo(ISLVARIABLE_CHECK_INFO, "Variable '%s':\n\tId: %s\n\tName: %s\n\tCausality: %s\n\tType (Size): %s (%d)"
		"\n\tConnection Id: %s\n\tInitial value: [%s]\n\tStep size: %gs\n\tStorage: %s\n\tSynchronisation timeout: %ds",
		m_sId.c_str(), m_sId.c_str(), m_sName.c_str(), GetCausalityAsStr().c_str(), GetType()->GetIdAsStr().c_str(),
		GetType()->GetSize(), m_sConnectId.c_str(), m_cType->GetInitialAsStr().c_str(), m_dStepSize,
		(m_bStore ? "true" : "false"), m_nSyncTimeout);
	m_bChecked = true;
	return true;
}

