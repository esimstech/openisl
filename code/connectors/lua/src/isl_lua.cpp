/* 
 *     Name: isl_lua.cpp
 *
 *     Description: LUA coupling for OpenISL.
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

#include <isl_api.h>
#include "logcodes.h"
#include "isl_lua.h"


/*
 *     Macros definition
 */

#if 1
#define LOCAL_DEBUG
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

/*
 *     Functions definition
 */

	static isl::CConnect * g_cConnect = 0;
	static std::string g_sGUID;


/*
 *     Functions definition
 */

	static int ISLInit(lua_State * L) {
		// ISLInit(string sModelName, string sSessionId, double dStartTime, double dEndTime, double dStepSize)
		const char * sModelName = luaL_checkstring(L, 1);
		const char * sSessionId = luaL_checkstring(L, 2);
		double dStartTime = luaL_checknumber(L, 3);
		double dEndTime = luaL_checknumber(L, 4);
		double dStepSize = luaL_checknumber(L, 5);
		int nRet = -1;
		//
		if (g_cConnect != 0) {
			// The instance already exists
			nRet = 1;
		}
		else {
			if (g_sGUID.empty() == false) {
				// Maybe an instance already exists
				g_cConnect = ISLInstances->Get(g_sGUID);
				if (g_cConnect != 0) {
					nRet = 2;
				}
			}
		}
		if (g_cConnect == 0) {
			g_cConnect = new isl::CConnect();
			g_cConnect->CloseLogOnDelete(true);
			if (g_cConnect->New(sModelName) == false) {
				ISLLogError(ERROR_CREATEMODEL_FAILED,
					"Failed to create the new connector instance: %s", sModelName);
				delete g_cConnect;
				g_cConnect = 0;
				lua_pushnumber(L, ERROR_CREATEMODEL_FAILED);
				return 1;
			}
			ISLLogInfo(99998, "DLL version: %s", GET_APP_VERSION(VERSION_NUMBER));
			g_cConnect->SetType("LUA");
			if (g_cConnect->SetSessionId(sSessionId) == false) {
				ISLLogError(ERROR_SETSESSIONID_FAILED, "Failed to set the session identifier: %s.", sSessionId);
				delete g_cConnect;
				g_cConnect = 0;
				lua_pushnumber(L, ERROR_SETSESSIONID_FAILED);
				return 1;
			}
			g_cConnect->SetStartTime(dStartTime);
			g_cConnect->SetEndTime(dEndTime);
			g_cConnect->SetStepSize(dStepSize);
			ISLLogInfo(INFO_MODEL_CREATED, "Model '%s' has been created.", g_cConnect->GetName().c_str());
			g_sGUID = isl::CConnect::GenUID();
			ISLInstances->Add(g_cConnect, g_sGUID);
			ISLInstances->SetDeleteAll(true);
			nRet = 0;
		}
		//
		lua_pushnumber(L, nRet);
		return 1; // Number of outputs returned
	}

	static int ISLConnect(lua_State * L) {
		// ISLConnect()
		if (g_cConnect == 0) {
			ISLLogError(ERROR_CONNECT_NOMODEL, "An ISL model is required to connect.");
			lua_pushnumber(L, ERROR_CONNECT_NOMODEL);
			return 1;
		}
		if (g_cConnect->IsConnnected()) {
			lua_pushnumber(L, 0);
			return 1;
		}
		if (g_cConnect->Check() == false) {
			ISLLogError(ERROR_CHECKMODEL_FAILED,
				"Failed to check the connector '%s' configuration.", g_cConnect->GetName().c_str());
			lua_pushnumber(L, ERROR_CHECKMODEL_FAILED);
			return 1;
		}
		if (g_cConnect->Create() == false) {
			ISLLogError(ERROR_CREATESESSION_FAILED, "Failed to create the session: %s.",
				g_cConnect->GetSessionId().c_str());
			lua_pushnumber(L, ERROR_CREATESESSION_FAILED);
			return 1;
		}
		ISLLogInfo(INFO_SESSION_CREATED, "The session '%s' has been created.",
			g_cConnect->GetSessionId().c_str());
		if (g_cConnect->Connect(true) == false) {
			ISLLogError(ERROR_CONNECT_FAILED, "Failed to connect the transmitters to the session '%s'.",
				g_cConnect->GetSessionId().c_str());
			lua_pushnumber(L, ERROR_CONNECT_FAILED);
			return 1;
		}
		ISLLogInfo(INFO_CONNECTEDTO_SESSION, "Connected to the transmitters.");
		//
		// TODO: Implement store
		//
		ISLLogInfo(INFO_ISLAPI_INITIALIZED, "ISL API: initialization done.");
		lua_pushnumber(L, 0);
		return 1;
	}

	static int ISLAddIO(lua_State * L) {
		// ISLAddIO(string sId, string sConnectId, double dInitVal, int nIsInput (0 = no, 1 = yes))
		const char * sId = luaL_checkstring(L, 1);
		const char * sConnectId = luaL_checkstring(L, 2);
		double dInitVal = luaL_checknumber(L, 3);
		bool bIsInput = luaL_checkinteger(L, 4) == 1;
		if (g_cConnect == 0) {
			ISLLogError(ERROR_ADDREALIO_NOMODEL, "The connector must be created first.");
			lua_pushnumber(L, ERROR_ADDREALIO_NOMODEL);
			return 1;
		}
		isl::CVariable::tCausality eCausality = isl::CVariable::CS_OUTPUT;
		if (bIsInput) {
			eCausality = isl::CVariable::CS_INPUT;
		}
		isl::CData * cData = g_cConnect->NewIO(sId, eCausality, isl::CDataType::TP_REAL);
		if (cData == 0) {
			ISLLogError(ERROR_NEWIO_FAILED, "Failed to create the IO: %s.", sId);
			lua_pushnumber(L, ERROR_NEWIO_FAILED);
			return 1;
		}
		cData->SetName(sId);
		cData->GetType()->Allocate();
		cData->GetType()->SetInitial(&dInitVal);
		cData->SetConnectId(sConnectId);
		lua_pushnumber(L, 0);
		return 1;
	}

	static int ISLGetDouble(lua_State * L) {
		// ISLGetDouble(sVarId: String, dCurrentTime: double, nWait (optional): 0 or 1)
		const char * sVarId = luaL_checkstring(L, 1);
		double dTime = luaL_checknumber(L, 2);
		int nWait = (int )(luaL_optinteger(L, 3, 1));
		if (g_cConnect == 0) {
			lua_pushnumber(L, ERROR_GETREAL_NOMODEL);
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);
			return 3;
		}
		isl::CData * cData = g_cConnect->GetInput(sVarId);
		if (cData == 0) {
			lua_pushnumber(L, ERROR_GET_NOACCESSTODATA);
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);
			return 3;
		}
		try {
			if (g_cConnect->IsTerminated()) {
				lua_pushnumber(L, WARNING_GET_ENDOFSIM_REACHED);
				lua_pushnumber(L, 0.0);
				lua_pushnumber(L, 0.0);
				return 3;
			}
			double dVal = 0.0;
			double dOutTime = dTime;
			if (cData->GetData(&dVal, &dOutTime, dTime, nWait == 1) == false) {
				lua_pushnumber(L, ERROR_GETDATA_FAILED);
				lua_pushnumber(L, 0.0);
				lua_pushnumber(L, 0.0);
				return 3;
			}
			lua_pushnumber(L, 0);
			lua_pushnumber(L, dVal);
			lua_pushnumber(L, dOutTime);
			//
			// TODO: Implement store
			//
		}
		catch (...) {
			lua_pushnumber(L, ERROR_GETREAL_FAILED);
			lua_pushnumber(L, 0.0);
			lua_pushnumber(L, 0.0);
			return 3;
		}
		return 3; // Number of results
	}

	static int ISLSetDouble(lua_State * L)	{
		// ISLSetDouble(sVarId: String, dValue: double, dCurrentTime: double, nWait (optional): 0 or 1)
		const char * sVarId = luaL_checkstring(L, 1);
		double dVal = luaL_checknumber(L, 2);
		double dTime = luaL_checknumber(L, 3);
		int nWait = (int)(luaL_optinteger(L, 4, 1));
		if (g_cConnect == 0) {
			lua_pushnumber(L, ERROR_SETREAL_NOMODEL);
			return 1;
		}
		isl::CData * cData = g_cConnect->GetOutput(sVarId);
		if (cData == 0) {
			lua_pushnumber(L, ERROR_SET_NOACCESSTODATA);
			return 1;
		}
		try {
			if (g_cConnect->IsTerminated()) {
				lua_pushnumber(L, WARNING_GET_ENDOFSIM_REACHED);
				return 1;
			}
			if (cData->SetData(&dVal, dTime, nWait == 1) == false) {
				lua_pushnumber(L, ERROR_SETDATA_FAILED);
				return 1;
			}
			lua_pushnumber(L, 0);
			//
			// TODO: Implement store
			//
		}
		catch (...) {
			lua_pushnumber(L, ERROR_SETREAL_FAILED);
			return 1;
		}
		return 1;
	}

	/*
	 * Registering functions
	 */

	static const struct luaL_Reg isl_lua[] = {
		{"init", ISLInit},
		{"connect", ISLConnect},
		{"addio", ISLAddIO},
		{"getdata", ISLGetDouble},
		{"setdata", ISLSetDouble},
		{NULL, NULL}
	};

	ISL_LUA_EXPORT int luaopen_isl_lua(lua_State* l) {
		luaL_newlibtable(l, isl_lua);
		luaL_setfuncs(l, isl_lua, 0);
		return 1;
	}

#ifdef __cplusplus
}
#endif // __cplusplus
