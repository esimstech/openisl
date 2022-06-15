//
//     Name: isl_cli.h
//
//     Description: OpenISL .Net API.
//
//     Modification date: 21/04/2022
//     Author: T. Roudier
//     Copyright (c) E-Sim Solutions Inc 2019-2022
//
//     Distributed under the OpenISL Software License, Version 1.0.
//
//

#pragma once

//
//     Using namespaces
//

using namespace System;
using namespace System::Collections::Generic;


//
//     Classes declaration
//

class isl::CData;
class isl::CConnect;

namespace isl_cli {

	public ref class CData
	{
	public:
		enum class tDataType {
			DATATYPE_NONE = -1,
			DATATYPE_REAL,
			DATATYPE_INTEGER,
			DATATYPE_BOOLEAN,
			DATATYPE_STRING
		};

		enum class tCausality {
			CAUSALITY_NONE = -1,
			CAUSALITY_INPUT,
			CAUSALITY_OUTPUT
		};

		CData(isl::CData * cData);
		~CData();

		String^ GetId();
		void SetName(String^ sName);
		String^ GetName();
		void SetConnectId(String^ sId);
		String^ GetConnectId();

		tDataType GetType();
		Int32 GetSize();

		Boolean IsInput();
		Boolean IsOutput();

		void SetFifoDepth(Int16 nDepth);
		Int16 GetFifoDepth();
		Int16 GetMaxFifoDepth();
		Int32 GetMaxReaders();

		void Allocate();
		Boolean SetInitial(Object^ pData);
		Boolean Initialize(Double dTime);

		Boolean SetData(Object^ pData, Double dTime, Boolean bWait);
		Boolean SetData(Object^ pData, Double dTime, Double dStep, Boolean bWait);

		Boolean GetData(Object^% pData, Double % dTime, Boolean bWait);
		Boolean GetData(Object^% pData, Double % dOutTime, Double dInTime, Boolean bWait);

		Boolean SetData(Object^ pData, Double dTime, Int32 nInd);
		Boolean GetData(Object^% pData, Double % dTime, Int32 nInd);

		Boolean StoreData(Object^ pData, Double dTime);

		Double GetOriginalStep();
		Double GetStepTolerance();

	private:
		isl::CData * m_cData;
		tDataType m_eType;
		tCausality m_eCausality;
	};

	public ref class CConnect
	{
	public:

		enum class tStopMode {
			STOPMODE_STOP = 0,
			STOPMODE_EXIT
		};

		static Boolean StartListenToExit(CConnect^ cObj);
		static Boolean SendStopRequest();
		static Boolean SendStopSession(String^ sSessionId);

		CConnect(Boolean bOwner);
		~CConnect();

		isl::CConnect * GetConnect();

		void CloseLogOnDelete(Boolean bVal);

		void SetMode(tStopMode eMode);
		tStopMode GetMode();

		Boolean ListenToExitSession();
		Boolean SendStopSession();
		Boolean IsTerminated();

		Boolean New(String^ sName);
		Boolean Load(String^ sFileName);
		Boolean Check();
		Boolean Save(String^ sFileName);

		void SetType(String^ sType);

		void SetStartTime(Double dVal);
		Double GetStartTime();
		void SetEndTime(Double dVal);
		Double GetEndTime();
		void SetStepSize(Double dVal);
		Double GetStepSize();

		CData^ NewIO(String^ sId, CData::tCausality eCausality, CData::tDataType eType, Int32 nSize);
		Int32 GetNbIOs();
		CData^ GetIO(String^ sId, Int32 % nInd);
		CData^ GetIO(Int32 i);

		Int32 GetNbInputs();
		CData^ GetInput(Int32 i);
		Int32 GetNbOutputs();
		CData^ GetOutput(Int32 i);

		Double GetStepTolerance();

		Boolean SetSessionId(String^ sId);
		String^ GetSessionId();

		Boolean Create(String^ sSessionId);
		Boolean Connect(Boolean bWait);
		Boolean Disconnect();

		Boolean OpenStore();
		Boolean CloseStore();

	private:
		isl::CConnect * m_cConnect;

		List<CData^> m_lData;
		List<CData^> m_lInputs;
		List<CData^> m_lOutputs;
	};

	public ref class CTools {
	public:
		static void PrintError(UInt32 uId, String^ sMsg);
		static void PrintWarning(UInt32 uId, String^ sMsg);
		static void PrintInfo(UInt32 uId, String^ sMsg);

		static UInt32 GetMaxStringSizeParameter();

		static Int32 GetMaxNbSimulations();
		static Boolean GetSimulation(Int32 nInd, UInt32 % uId, UInt32 % ulPID);
	};
}
