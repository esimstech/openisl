/*
 *     Name: isl_variable.h
 *
 *     Description: ISL API Variable class.
 *
 *     Modification date: 16/04/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2019-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
 *
 */

#ifndef _ISL_VARIABLE_H_
#define _ISL_VARIABLE_H_

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

	class ISL_API_EXPORT CDataType
	{
	public:
		enum tType {
			TP_REAL,
			TP_INTEGER,
			TP_BOOLEAN,
			TP_STRING,
			TP_STRUCTURE,
			TP_UNKNOWN
		};

		CDataType(tType eType, int nSize);
		~CDataType();

		bool AddSubType(const std::string & sField, tType eType, int nSize);
		CDataType * GetSubType(const std::string & sField);
		CDataType * GetSubType(int i);
		int GetNbOfSubTypes();

		tType GetId();
		std::string GetIdAsStr();
		void SetLabel(const std::string & sVal);
		std::string GetLabel();

		int GetSize(); // return the number of elements
		int GetSizeOf(); // return the size of one element in bytes
		int GetSizeInBytes(); // return the size of the variable (all elements) in bytes

		void * GetInitial();
		std::string GetInitialAsStr();
		void * GetValue();

		bool SetInitial(const std::string & sVal);
		bool SetInitial(void * pVal);
		bool SetValue(void * pVal); // Maybe not useful (direct access to the SHM)

		void Allocate();
		void Free();
		bool IsAllocated();

		static bool IsValidType(tType eType, bool bIsSubType = false);

	private:
		static int GetSizeOf(tType eType);

	private:
		tType m_eType;
		std::string m_sLabel; // Only used for structured types
		int m_nSize;
		int m_nSizeOf;
		int m_nSizeInBytes;

		void * m_pInitial;
		void * m_pValue;

		bool m_bAllocated;

		std::map<std::string, CDataType *> m_mSubTypes;
		std::vector<CDataType *> m_lSubTypes;
		void ** m_pSubInitials;
		void ** m_pSubValues;
	};

	class ISL_API_EXPORT CVariable
	{
	public:
		// TODO: To consider: ISL_PARAMETER
		enum tCausality {
			CS_INPUT,
			CS_OUTPUT,
			CS_UNKNOWN
		};

		enum tState {
			ST_NOT_DEFINED,
			ST_PARTIALLY_DEFINED,
			ST_FULLY_DEFINED,
			ST_IS_CONNECTED
		};

		CVariable(CConnect * cParent, const std::string & sId);
		~CVariable();

		tState GetState();
		CConnect * GetParent();

		std::string GetId();
		void SetName(const std::string & sName);
		std::string GetName();
		void SetConnectId(const std::string & sId);
		std::string GetConnectId();

		void SetCausality(const std::string & sVal);
		void SetCausality(tCausality eCausality);
		tCausality GetCausality();
		std::string GetCausalityAsStr();
		bool IsOutput();
		bool IsInput();

		CDataType::tType GetTypeId();
		CDataType * GetType();
		bool SetType(CDataType::tType eType, int nSize);

		void SetStepSize(double dStep);
		void UseStore(bool bUse);
		bool IsStoreUsed();
		void SetSyncTimeout(int nTimeout); // Set in seconds
		int GetSyncTimeout();

		bool Check();

	protected:
		virtual void InitOnCheck() = 0;

	protected:
		CConnect * m_cParent;
		tState m_eState;

		std::string m_sName;
		std::string m_sId;

		std::string m_sConnectId;

		tCausality m_eCausality;
		CDataType * m_cType;

		double m_dStepSize;
		bool m_bStore;
		int m_nSyncTimeout; // Used by semaphores (in milliseconds)

		// TODO: Implement CComputeSettings (map of settings)
	};

}

#endif // _ISL_VARIABLE_H_
