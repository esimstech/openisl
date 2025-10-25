/* 
 *     Name: islfmulave.h
 *
 *     Description: FMI2 Slave coupling for ISL.
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

#ifndef _ISLFMUSLAVE_H_
#define _ISLFMUSLAVE_H_

#include <isl_api.h>
#include <vector>
#include <map>
#include <set>
#include <string>


/*
 *     Declarative section
 */

typedef struct {
	std::string m_sName;
	unsigned int m_uRef;
	isl::CDataType::tType m_eType;
	isl::CData * m_cData;
} ISLFMUData;

typedef std::map<int, ISLFMUData *> CISLFMUDataMap;

class ISLFMUSlave
{
public:
	static ISLFMUSlave* Create();

	// Load the FMX, and prepare IOs
	// FMX should be compatible with the modelDescription
	ISLFMUSlave();

	// Close the co-simulation, delete the connector
	~ISLFMUSlave();

	// Create or connect to the session
	// Initialize IOs
	void Init();
	void EndInit();

	// Terminate the simulation
	void Terminate();

	void SetReal(int varID, double value);

	void SetInt(int varID, int value);

	void SetString(int varID, fmi2String value);

	void SetBool(int varID, bool value);

	void GetReal(int varID, double& value);

	void GetInt(int varID, int& value);

	void GetString(int varID, fmi2String& value);

	void GetBool(int varID, bool& value);

	// Read inputs (FMU)
	// Send them as outputs (FMX)
	virtual void IntegrateTo(double currentCommunicationPoint, double communicationStepSize);

	void Logger(fmi2Status state, fmi2String category, fmi2String msg);

	void Logger(fmi2Status state, fmi2String category, const std::string& msg);

private:
	bool ISLSetup();
	bool ISLTerminate();

	bool GetDataAndSend();
	bool ReceiveAndSetData();

private:
	// ISL connector
	isl::CConnect * m_cConnect;

	// ISL Data
	CISLFMUDataMap m_mIOs;
	std::vector<int> m_lInRefs;
	std::vector<int> m_lOutRefs;

	std::string m_sErrorMsg;

	// Local information of the current step
	double m_currentTimePoint;

	std::string m_lastError;

	std::map<int, int> m_boolVar;
	std::map<int, double> m_realVar;
	std::map<int, int> m_integerVar;
	std::map<int, std::string> m_stringVar;

	bool m_alreadyCleaned;

public:
	std::string m_GUID;
	std::string m_instanceName;
	std::string m_resourceLocation;

	const fmi2CallbackFunctions* m_callbackFunctions;
	bool m_initializationMode;

	bool m_isCosimulation;

	bool m_loggingOn;
	std::vector<std::string> m_loggingCategories;
};

#endif // _ISLFMUSLAVE_H_

