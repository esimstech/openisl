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

		bool IsInitialSet();
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
		bool m_bInitialSet;

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

		bool IsChecked();
		bool Check();

	protected:
		virtual void InitOnCheck() = 0;

	protected:
		CConnect * m_cParent;
		tState m_eState;
		bool m_bChecked;

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
