# 
#     Name: CISLConnect.py
#
#     Description: Python OpenISL connector.
#
#     Author: T. Roudier
#     Copyright (c) 2019-2025 E-Sim Solutions Inc
#
#     Distributed under the MIT License.
# 
#     --------------------------------------------------------------------------
# 
#     Permission is hereby granted, free of charge, to any person obtaining a
#     copy of this software and associated documentation files (the “Software”),
#     to deal in the Software without restriction, including without limitation
#     the rights to use, copy, modify, merge, publish, distribute, sublicense,
#     and/or sell copies of the Software, and to permit persons to whom the
#     Software is furnished to do so, subject to the following conditions:
#
#     The above copyright notice and this permission notice shall be included in
#     all copies or substantial portions of the Software.
#
#     THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
#     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#     DEALINGS IN THE SOFTWARE.
# 
#     --------------------------------------------------------------------------
#
#

import sys

from pathlib import Path
from ctypes import *
from openisl import CISLLib
from openisl import CISLData
from openisl.ISLUtils import *

ISLLib = CISLLib.CISLLib()

class CISLConnect(object):
    """ISL Connector"""

    def __init__(self, bOwner):
        self.__m_bOwner = bOwner
        self.__m_bSignalsAllocated = False
        self.__m_sName = ''
        self.__m_sFile = ''
        self.m_cIns = {}
        self.m_cOuts = {}
        nOwner = int(self.__m_bOwner)
        try:
            self.__m_cConnect = ISLLib.ConnectInit(nOwner)
            if self.__m_cConnect == None:
                ISLLogError(2000, "Failed to build a new instance of a connector.")
        except:
            e = sys.exc_info()
            ISLLogError(2001, e[0], ": ", e[1])

    def __del__(self):
        self.Free()

    def __AllocateSignals(self):
        # Used by Load
        if self.__m_cConnect == None:
            ISLLogError(2030, "No instance of ISL connector.")
            return False
        if self.__m_bSignalsAllocated:
            return True;
        bRet = True
        nNbSigs = ISLLib.ConnectGetNbIOs(self.__m_cConnect)
        for i in range(nNbSigs):
            try:
                cData = ISLLib.ConnectGetIO(self.__m_cConnect, i)
                sId = ISLLib.IOGetId(cData).decode('utf-8')
                nType = ISLLib.IOGetTypeId(cData)
                cSig = None
                if nType == 0: # TP_REAL
                    cSig = CISLData.CISLFloat(self)
                elif nType == 1: # TP_INTEGER
                    cSig = CISLData.CISLInt(self)
                elif nType == 2: # TP_BOOLEAN
                    cSig = CISLData.CISLBool(self)
                elif nType == 3: # TP_STRING
                    cSig = CISLData.CISLString(self)
                elif nType == 4: # TP_STRUCTURE
                    ISLLogError(2087, "Variable '", sId, "': 'Structure' types are not yet managed")
                    return False
                else:
                    ISLLogError(2031, "Variable '", sId,"': Failed to get a valid data type(" , nType,")")
                    bRet = False
                if cSig != None:
                    cSig.SetInternalData(cData)
                    if ISLLib.IOIsInput(cData) == 1:
                        self.m_cIns[sId] = cSig
                    else:
                        self.m_cOuts[sId] = cSig
            except:
                e = sys.exc_info()
                ISLLogError(2032, "[", i,"]: ", e[0], ": ", e[1])
                bRet = False
        self.__m_bSignalsAllocated = True
        if bRet:
            ISLLogInfo(2033, "ISL variables have been locally mapped.")
        return bRet

    def Free(self):
        try:
            del self.m_cIns
        finally:
            self.m_cIns = {}
        try:
            del self.m_cOuts
        finally:
            self.m_cOuts = {}
        if self.__m_cConnect != None:
            try:
                ISLLogInfo(2007, "Unloading the connector")
                if ISLLib.ConnectFree(self.__m_cConnect) != 0:
                    ISLLogError(2008, "Failed to delete the connector instance.")
            except:
                e = sys.exc_info()
                ISLLogError(2009, e[0], ": ", e[1])
            finally:
                self.__m_cConnect = None

    def CloseLogOnDelete(self, bVal):
        if self.__m_cConnect == None:
            ISLLogError(2107, "No instance of ISL connector.")
            return False
        try:
            nVal = 0
            if bVal:
                nVal = 1
            return ISLLib.ConnectCloseLogOnDelete(self.__m_cConnect, nVal) == 0
        except:
            e = sys.exc_info()
            ISLLogError(2108, e[0], ": ", e[1])
            return False

    def New(self, sName):
        if self.__m_cConnect == None:
            ISLLogError(2002, "No instance of ISL connector.")
            return False
        if sName:
            self.__m_sName = sName
        if not self.__m_sName:
            ISLLogError(2003, "No identifier (name) has been defined.")
            return False
        try:
            nRet = ISLLib.ConnectNew(self.__m_cConnect, self.__m_sName.encode('utf-8'))
            if nRet != 0:
                ISLLogError(2004, "Failed to initialize the new instance.")
                return False
            ISLLogInfo(2005, "A new instance '", self.__m_sName, "' has been initialized.");
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2006, e[0], ": ", e[1])
            return False
        
    def Load(self, sFileName):
        if self.__m_cConnect == None:
            ISLLogError(2010, "No instance of ISL connector.")
            return False
        if sFileName:
            self.__m_sFile = sFileName
        if not self.__m_sFile:
            ISLLogError(2011, "No input file to load.")
            return False
        try:
            nRet = ISLLib.ConnectLoad(self.__m_cConnect, self.__m_sFile.encode('utf-8'))
            if nRet != 0:
                ISLLogError(2012, "Failed to load the file '", self.__m_sFile, "'")
                return False
            ISLLogInfo(2013, "The file '", self.__m_sFile,"' has been loaded.")
            return self.__AllocateSignals();
        except:
            e = sys.exc_info()
            ISLLogError(2014, e[0], ": ", e[1])
            return False

    def Check(self):
        if self.__m_cConnect == None:
            ISLLogError(2015, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectCheck(self.__m_cConnect)
            if nRet != 0:
                ISLLogError(2016, "Failed to validate the connector instance: '", self.__m_sName, "'")
                return False
            ISLLogInfo(2017, "The connector instance '", self.__m_sName,"' has been validated.")
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2018, e[0], ": ", e[1])
            return False

    def Save(self, sFileName):
        if self.__m_cConnect == None:
            ISLLogError(2019, "No instance of ISL connector.")
            return False
        if (not sFileName) and (not self.__m_sFile):
            ISLLogError(2020, "No file name has been defined.")
            return False
        if (not self.__m_sFile) and sFileName:
            self.__m_sFile = sFileName
        try:
            nRet = ISLLib.ConnectSave(self.__m_cConnect, sFileName.encode('utf-8'))
            if nRet != 0:
                ISLLogError(2021, "Failed to save the connector configuration in '", sFileName, "'")
                return False
            ISLLogInfo(2022, "The file '", sFileName,"' has been saved.")
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2023, e[0], ": ", e[1])
            return False

    def SetName(self, sName):
        if self.__m_cConnect == None:
            ISLLogError(2024, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetName(self.__m_cConnect, sName.encode('utf-8'))
            if nRet != 0:
                ISLLogError(2025, "Failed to set the name: '", sName, "'")
                return False
            self.__m_sName = sName
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2026, e[0], ": ", e[1])
            return False

    def GetName(self):
        if self.__m_cConnect == None:
            ISLLogError(2027, "No instance of ISL connector.")
            return None
        try:
            sName = ISLLib.ConnectGetName(self.__m_cConnect).decode('utf-8')
            self.__m_sName = sName
            return self.__m_sName;
        except:
            e = sys.exc_info()
            ISLLogError(2029, e[0], ": ", e[1])
            return None

    def GetId(self):
        if self.__m_cConnect == None:
            ISLLogError(2034, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetId(self.__m_cConnect).decode('utf-8')
        except:
            e = sys.exc_info()
            ISLLogError(2036, e[0], ": ", e[1])
            return None

    def GetUId(self):
        if self.__m_cConnect == None:
            ISLLogError(2028, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetUId(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2035, e[0], ": ", e[1])
            return None

    def SetType(self, sType):
        if self.__m_cConnect == None:
            ISLLogError(2037, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetType(self.__m_cConnect, sType.encode('utf-8'))
            if nRet != 0:
                ISLLogError(2038, "Failed to set the connector type: '", sType, "'")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2039, e[0], ": ", e[1])
            return False

    def GetType(self):
        if self.__m_cConnect == None:
            ISLLogError(2040, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetTypeAsStr(self.__m_cConnect).decode('utf-8')
        except:
            e = sys.exc_info()
            ISLLogError(2041, e[0], ": ", e[1])
            return None

    def GetTypeId(self):
        if self.__m_cConnect == None:
            ISLLogError(2042, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetType(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2043, e[0], ": ", e[1])
            return None

    def SetPID(self, ulPID):
        if self.__m_cConnect == None:
            ISLLogError(2044, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetPID(self.__m_cConnect, ulPID)
            if nRet != 0:
                ISLLogError(2045, "Failed to set the process identifier: ", ulPID)
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2046, e[0], ": ", e[1])
            return False

    def GetPID(self):
        if self.__m_cConnect == None:
            ISLLogError(2047, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetPID(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2048, e[0], ": ", e[1])
            return None

    def GetFileName(self):
        if self.__m_cConnect == None:
            ISLLogError(2049, "No instance of ISL connector.")
            return None
        try:
            sFile = ISLLib.ConnectGetFileName(self.__m_cConnect).decode('utf-8')
            self.__m_sFile = sFile
            return self.__m_sFile;
        except:
            e = sys.exc_info()
            ISLLogError(2050, e[0], ": ", e[1])
            return None

    def SetSessionId(self, sId):
        if self.__m_cConnect == None:
            ISLLogError(2051, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetSessionId(self.__m_cConnect, sId.encode('utf-8'))
            if nRet != 0:
                ISLLogError(2052, "Failed to set the session identifier: '", sId, "'")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2053, e[0], ": ", e[1])
            return False

    def GetSessionId(self):
        if self.__m_cConnect == None:
            ISLLogError(2054, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetSessionId(self.__m_cConnect).decode('utf-8')
        except:
            e = sys.exc_info()
            ISLLogError(2055, e[0], ": ", e[1])
            return None

    def SetConnectTimeOut(self, nVal):
        if self.__m_cConnect == None:
            ISLLogError(2056, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetConnectTimeOut(self.__m_cConnect, nVal)
            if nRet != 0:
                ISLLogError(2057, "Failed to set the connection time out")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2058, e[0], ": ", e[1])
            return False

    def GetConnectTimeOut(self):
        if self.__m_cConnect == None:
            ISLLogError(2059, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetConnectTimeOut(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2060, e[0], ": ", e[1])
            return None

    def SetStartTime(self, dVal):
        if self.__m_cConnect == None:
            ISLLogError(2061, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetStartTime(self.__m_cConnect, dVal)
            if nRet != 0:
                ISLLogError(2062, "Failed to set the start time")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2063, e[0], ": ", e[1])
            return False

    def GetStartTime(self):
        if self.__m_cConnect == None:
            ISLLogError(2064, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetStartTime(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLPrintError(2065, e[0], ": ", e[1])
            return None

    def SetEndTime(self, dVal):
        if self.__m_cConnect == None:
            ISLLogError(2066, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetEndTime(self.__m_cConnect, dVal)
            if nRet != 0:
                ISLLogError(2067, "Failed to set the end time")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2068, e[0], ": ", e[1])
            return False

    def GetEndTime(self):
        if self.__m_cConnect == None:
            ISLLogError(2069, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetEndTime(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLPrintError(2070, e[0], ": ", e[1])
            return None

    def SetStepSize(self, dVal):
        if self.__m_cConnect == None:
            ISLLogError(2071, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetStepSize(self.__m_cConnect, dVal)
            if nRet != 0:
                ISLLogError(2072, "Failed to set the step size")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2073, e[0], ": ", e[1])
            return False

    def GetStepSize(self):
        if self.__m_cConnect == None:
            ISLLogError(2074, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetStepSize(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLPrintError(2075, e[0], ": ", e[1])
            return 0.0

    def SetStepTolerance(self, dVal):
        if self.__m_cConnect == None:
            ISLLogError(2076, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSetStepTolerance(self.__m_cConnect, dVal)
            if nRet != 0:
                ISLLogError(2077, "Failed to set the step tolerance")
                return False
            return True;
        except:
            e = sys.exc_info()
            ISLLogError(2078, e[0], ": ", e[1])
            return False

    def GetStepTolerance(self):
        if self.__m_cConnect == None:
            ISLLogError(2079, "No instance of ISL connector.")
            return None
        try:
            return ISLLib.ConnectGetStepTolerance(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLPrintError(2080, e[0], ": ", e[1])
            return 0.0

    def NewIO(self, sId, nCausality = 0, nType = 0, nSize = 1):
        if self.__m_cConnect == None:
            ISLLogError(2081, "No instance of ISL connector.")
            return False
        try:
            cData = ISLLib.ConnectNewIO(self.__m_cConnect, sId.encode('utf-8'), nCausality, nType, nSize)
            if not cData:
                ISLLogError(2082, "Failed to create a new IO")
                return False
            cSig = None
            if nType == 0: # TP_REAL
                cSig = CISLData.CISLFloat(self)
            elif nType == 1: # TP_INTEGER
                cSig = CISLData.CISLInt(self)
            elif nType == 2: # TP_BOOLEAN
                cSig = CISLData.CISLBool(self)
            elif nType == 3: # TP_STRING
                cSig = CISLData.CISLString(self)
            elif nType == 4: # TP_STRUCTURE
                ISLLogError(2088, "Variable '", sId, "': 'Structure' types are not yet managed")
                return False
            else:
                ISLLogError(2083, "Variable '", sId,"': Failed to get a valid data type(" , nType,")")
                return False
            if cSig != None:
                cSig.SetInternalData(cData)
                if ISLLib.IOIsInput(cData) == 1:
                    self.m_cIns[sId] = cSig
                else:
                    self.m_cOuts[sId] = cSig
                return True
            return False;
        except:
            e = sys.exc_info()
            ISLLogError(2084, e[0], ": ", e[1])
            return False

    def RemoveIO(self, sId):
        if self.__m_cConnect == None:
            ISLLogError(2085, "No instance of ISL connector.")
            return False
        try:
            cIO = None
            if sId in self.m_cIns.keys():
                cIO = self.m_cIns.pop(sId)
            elif sId in self.m_cOuts.keys():
                cIO = self.m_cOuts.pop(sId)
            if cIO:
                cData = cIO.GetInternalData()
                if cData:
                    if ISLLib.ConnectRemoveIO(self.__m_cConnect, cData) == 0:
                        return True
                    ISLLogError(2089, "Failed to remove variable '", sId, "'")
                    return False
                ISLLogError(2090, "Failed to get the ISL variable '", sId, "'")
                return False
            ISLLogError(2091, "Variable '", sId, "' not found.")
            return False
        except:
            e = sys.exc_info()
            ISLLogError(2086, e[0], ": ", e[1])
            return False

    def GetNbIOs(self):
        if self.__m_cConnect == None:
            ISLLogError(2092, "No instance of ISL connector.")
            return 0
        try:
            return ISLLib.ConnectGetNbIOs(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2093, e[0], ": ", e[1])
            return 0

    def GetNbInputs(self):
        if self.__m_cConnect == None:
            ISLLogError(2094, "No instance of ISL connector.")
            return 0
        try:
            return ISLLib.ConnectGetNbInputs(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2095, e[0], ": ", e[1])
            return 0

    def GetNbOutputs(self):
        if self.__m_cConnect == None:
            ISLLogError(2096, "No instance of ISL connector.")
            return 0
        try:
            return ISLLib.ConnectGetNbOutputs(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2097, e[0], ": ", e[1])
            return 0

    def IOExists(self, sId):
        if self.__m_cConnect == None:
            ISLLogError(2098, "No instance of ISL connector.")
            return False
        try:
            return ISLLib.ConnectIOExists(self.__m_cConnect, sId.encode('utf-8')) == 1
        except:
            e = sys.exc_info()
            ISLLogError(2099, e[0], ": ", e[1])
            return False

    def GetIOFromIndex(self, i):
        if self.__m_cConnect == None:
            ISLLogError(2100, "No instance of ISL connector.")
            return None
        try:
            cData = ISLLib.ConnectGetIO(self.__m_cConnect, i)
            if cData:
                sId = ISLLib.IOGetId(cData).decode('utf-8')
                if ISLLib.IOIsInput(cData) == 1:
                    return self.m_cIns.get(sId, None)
                elif ISLLib.IOIsOutput(cData) == 1:
                    return self.m_cOuts.get(sId, None)
            return None
        except:
            e = sys.exc_info()
            ISLLogError(2101, e[0], ": ", e[1])
            return None

    def GetIOFromId(self, sId):
        try:
            if sId in self.m_cIns.keys():
                return self.m_cIns.get(sId, None)
            elif sId in self.m_cOuts.keys():
                return self.m_cOuts.get(sId, None)
            return None
        except:
            e = sys.exc_info()
            ISLLogError(2102, e[0], ": ", e[1])
            return None

    def GetInputFromIndex(self, i):
        if self.__m_cConnect == None:
            ISLLogError(2103, "No instance of ISL connector.")
            return None
        try:
            cData = ISLLib.ConnectGetInput(self.__m_cConnect, i)
            if cData:
                sId = ISLLib.IOGetId(cData).decode('utf-8')
                return self.m_cIns.get(sId, None)
            return None
        except:
            e = sys.exc_info()
            ISLLogError(2104, e[0], ": ", e[1])
            return None

    def GetOutputFromIndex(self, i):
        if self.__m_cConnect == None:
            ISLLogError(2105, "No instance of ISL connector.")
            return None
        try:
            cData = ISLLib.ConnectGetOutput(self.__m_cConnect, i)
            if cData:
                sId = ISLLib.IOGetId(cData).decode('utf-8')
                return self.m_cOuts.get(sId, None)
            return None
        except:
            e = sys.exc_info()
            ISLLogError(2106, e[0], ": ", e[1])
            return None

    def SetMode(self, nMode):
        # STOP : 0
        # EXIT : 1
        if self.__m_cConnect == None:
            ISLLogError(2109, "No instance of ISL connector.")
            return False
        try:
            return ISLLib.ConnectSetMode(self.__m_cConnect, nMode) == 0
        except:
            e = sys.exc_info()
            ISLLogError(2110, e[0], ": ", e[1])
            return False

    def GetMode(self):
        # STOP : 0
        # EXIT : 1
        if self.__m_cConnect == None:
            ISLLogError(2111, "No instance of ISL connector.")
            return -1
        try:
            return ISLLib.ConnectGetMode(self.__m_cConnect)
        except:
            e = sys.exc_info()
            ISLLogError(2112, e[0], ": ", e[1])
            return -1

    def SetViewer(self, bVal):
        if self.__m_cConnect == None:
            ISLLogError(2113, "No instance of ISL connector.")
            return False
        try:
            nVal = int(bVal)
            return ISLLib.ConnectSetViewer(self.__m_cConnect, nVal) == 0
        except:
            e = sys.exc_info()
            ISLLogError(2114, e[0], ": ", e[1])
            return False

    def IsViewer(self):
        if self.__m_cConnect == None:
            ISLLogError(2115, "No instance of ISL connector.")
            return False
        try:
            return ISLLib.ConnectIsViewer(self.__m_cConnect) == 1
        except:
            e = sys.exc_info()
            ISLLogError(2116, e[0], ": ", e[1])
            return False

    def SetTerminated(self, bVal):
        if self.__m_cConnect == None:
            ISLLogError(2117, "No instance of ISL connector.")
            return False
        try:
            nVal = int(nVal)
            return ISLLib.ConnectSetTerminated(self.__m_cConnect, nVal) == 0
        except:
            e = sys.exc_info()
            ISLLogError(2118, e[0], ": ", e[1])
            return False

    def IsTerminated(self):
        if self.__m_cConnect == None:
            return True
        try:
            return ISLLib.ConnectIsTerminated(self.__m_cConnect) == 1
        except:
            return True

    def Create(self, sSessionId = ''):
        if self.__m_cConnect == None:
            ISLLogError(2119, "No instance of ISL connector.")
            return False
        if sSessionId:
            if not self.SetSessionId(sSessionId):
                return False
        try:
            nRet = ISLLib.ConnectCreate(self.__m_cConnect, None)
            if nRet != 0:
                ISLLogError(2120, "Failed to create a session. Error code: ", nRet)
                return False
            ISLLogInfo(2122, "Session '", self.GetSessionId(), "' created.")
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2121, e[0], ": ", e[1])
            return False

    def Connect(self, bWait):
        if self.__m_cConnect == None:
            ISLLogError(2123, "No instance of ISL connector.")
            return False
        nWait = int(bWait)
        try:
            nRet = ISLLib.ConnectConnect(self.__m_cConnect, nWait)
            if nRet != 0:
                ISLLogError(2124, "Failed to connect to the transmitters. Error code: ", nRet)
                return False
            ISLLogInfo(2125, "Connected to the transmitters.")
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2126, e[0], ": ", e[1])
            return False

    def Disconnect(self):
        if self.__m_cConnect == None:
            ISLLogError(2127, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectDisconnect(self.__m_cConnect)
            if nRet != 0:
                ISLLogError(2128, "Failed to disconnect from the session. Error code: ", nRet)
                return False
            ISLLogInfo(2129, "Disconnected from the session.")
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2130, e[0], ": ", e[1])
            return False

    def StartThreadListenToStop(self):
        if self.__m_cConnect == None:
            ISLLogError(2131, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.StartThreadListenToStop(self.__m_cConnect)
            if nRet != 0:
                ISLLogError(2132, "StartThreadListenToStop Failed. Error code: ", nRet)
                return False
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2133, e[0], ": ", e[1])
            return False

    def ListenToExitSession(self):
        if self.__m_cConnect == None:
            ISLLogError(2134, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectListenToExitSession(self.__m_cConnect)
            if nRet != 0:
                ISLLogError(2135, "ListenToExitSession Failed. Error code: ", nRet)
                return False
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2136, e[0], ": ", e[1])
            return False

    def SendStopSession(self):
        if self.__m_cConnect == None:
            ISLLogError(2137, "No instance of ISL connector.")
            return False
        try:
            nRet = ISLLib.ConnectSendStopSession(self.__m_cConnect)
            if nRet != 0:
                ISLLogError(2138, "SendStopSession Failed. Error code: ", nRet)
                return False
            return True
        except:
            e = sys.exc_info()
            ISLLogError(2139, e[0], ": ", e[1])
            return False

    def OpenStore(self):
        ISLLogError(2140, "Not yet implemented");
        return False

    def CloseStore(self):
        ISLLogError(2141, "Not yet implemented");
        return False
