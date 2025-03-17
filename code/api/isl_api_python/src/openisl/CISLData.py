# 
#     Name: CISLData.py
#
#     Description: Python OpenISL variable.
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
import threading

from ctypes import *
from openisl import CISLLib
from openisl.ISLUtils import *

ISLLib = CISLLib.CISLLib()

class CISLData(object):
    """ISL Data"""

    def __init__(self, cParent):
        self.__m_cData = None
        self.__m_cParent = cParent
        self.m_cValue = None
        self.m_nStatus = 0
        self.m_sId = ''
        self.m_nType = -1

    def __del__(self):
        ISLLogInfo(3000, "Unloading signal '", self.m_sId, "'")
        self.__m_cData = None
        self.__m_cParent = None
        self.m_cValue = None
        self.m_nStatus = 0
        self.m_sId = ''
        self.m_nType = -1

    def SetInternalData(self, cData):
        if cData == None:
            return False
        self.__m_cData = cData
        try:
            self.m_sId = ISLLib.IOGetId(self.__m_cData).decode('utf-8')
            return True
        except:
            e = sys.exc_info()
            ISLLogError(3001, e[0], ": ", e[1])
            return False
        return True
    
    def GetInternalData(self):
        return self.__m_cData

    def SetCausality(self, nCausality):
        try:
            if ISLLib.IOSetCausality(self.__m_cData, nCausality) != 0:
                return False
        except:
            return False
        return True

    def GetCausality(self):
        try:
            return ISLLib.IOGetCausality(self.__m_cData)
        except:
            return -1

    def SetName(self, sName):
        try:
            if ISLLib.IOSetName(self.__m_cData, sName.encode('utf-8')) != 0:
                return False
        except:
            return False
        return True

    def GetName(self):
        try:
            return ISLLib.IOGetName(self.__m_cData).decode('utf-8')
        except:
            return None

    def SetConnectId(self, sId):
        try:
            if ISLLib.IOSetConnectId(self.__m_cData, sId.encode('utf-8')) != 0:
                return False
        except:
            return False
        return True

    def GetConnectId(self):
        try:
            return ISLLib.IOGetConnectId(self.__m_cData).decode('utf-8')
        except:
            return None

    def IsInput(self):
        try:
            return ISLLib.IOIsInput(self.__m_cData) == 1
        except:
            return False

    def IsOutput(self):
        try:
            return ISLLib.IOIsOutput(self.__m_cData) == 1
        except:
            return False

    def SetStepSize(self, dStepSize):
        try:
            if ISLLib.IOSetStepSize(self.__m_cData, dStepSize) == 0:
                return True
        except:
            return False
        return False

    def UseStore(self, bVal):
        try:
            nVal = int(bVal)
            if ISLLib.IOUseStore(self.__m_cData, nVal) == 0:
                return True
        except:
            return False
        return False

    def IsStoreUsed(self):
        try:
            return ISLLib.IOIsStoreUsed(self.__m_cData) == 1
        except:
            return False

    def SetSyncTimeout(self, nTimeout):
        try:
            if ISLLib.IOSetSyncTimeout(self.__m_cData, nTimeout) == 0:
                return True
        except:
            return False
        return False

    def GetSyncTimeout(self, nTimeout):
        try:
            return ISLLib.IOGetSyncTimeout(self.__m_cData)
        except:
            return -1

    def IsTerminated(self):
        try:
            return self.__m_cParent.IsTerminated()
        except:
            return False

    def SetInitial(self):
        try:
            cType = ISLLib.IOGetType(self.__m_cData);
            if not cType:
                return False
            self.m_nStatus = ISLLib.TypeIsAllocated(cType)
            if self.m_nStatus == 0:
                self.m_nStatus = ISLLib.TypeAllocate(cType)
            if self.m_nStatus == -1:
                return False
            self.m_nStatus = ISLLib.TypeSetInitial(cType, self.m_cValue)
            if self.m_nStatus == 0:
                return True
        except:
            return False
        return False

    def SetData(self, dTime, bWait):
        try:
            self.m_nStatus = ISLLib.IOSetData(self.__m_cData, self.m_cValue, dTime, int(bWait))
            if self.m_nStatus == 0:
                return True
        except:
            return False
        return False

    def SetDataAndStep(self, dTime, dStep, bWait):
        try:
            self.m_nStatus = ISLLib.IOSetDataAndNextStep(self.__m_cData, self.m_cValue, dTime, dStep, int(bWait))
            if self.m_nStatus == 0:
                return True
        except:
            return False
        return False

    def GetLastData(self, bWait):
        dTmpTime = c_double(0.0)
        cTmpTime = pointer(dTmpTime)
        try:
            self.m_nStatus = ISLLib.IOGetLastData(self.__m_cData, self.m_cValue, cTmpTime, int(bWait))
            if self.m_nStatus == 0:
                return True, dTmpTime.value
        except:
            return False, 0.0
        return False, 0.0

    def GetData(self, dTime, bWait):
        dTmpTime = c_double(0.0)
        cTmpTime = pointer(dTmpTime)
        try:
            self.m_nStatus = ISLLib.IOGetData(self.__m_cData, self.m_cValue, cTmpTime, dTime, int(bWait))
            if self.m_nStatus == 0:
                return True, dTmpTime.value
        except:
            return False, 0.0
        return False, 0.0

    def SetEventData(self, bWait):
        try:
            self.m_nStatus = ISLLib.IOSetEventData(self.__m_cData, self.m_cValue, int(bWait))
            if self.m_nStatus == 0:
                return True
        except:
            return False
        return False

    def GetEventData(self, bWait):
        try:
            self.m_nStatus = ISLLib.IOGetEventData(self.__m_cData, self.m_cValue, int(bWait))
            if self.m_nStatus == 0:
                return True
        except:
            return False
        return False

    def SetDataAt(self, dTime, nInd):
        try:
            self.m_nStatus = ISLLib.IOSetDataAt(self.__m_cData, self.m_cValue, dTime, nInd)
            if self.m_nStatus == 0:
                return True
        except:
            return False
        return False

    def GetDataAt(self, nInd):
        dTmpTime = c_double(0.0)
        cTmpTime = pointer(dTmpTime)
        try:
            self.m_nStatus = ISLLib.IOGetDataAt(self.__m_cData, self.m_cValue, cTmpTime, nInd)
            if self.m_nStatus == 0:
                return True, dTmpTime.value
        except:
            return False, 0.0
        return False, 0.0

    def GetMemData(self, nInd):
        dTmpTime = c_double(0.0)
        dTmpStep = c_double(0.0)
        cTmpTime = pointer(dTmpTime)
        cTmpStep = pointer(dTmpStep)
        try:
            self.m_nStatus = ISLLib.IOGetMemData(self.__m_cData, self.m_cValue, cTmpTime, cTmpStep, nInd)
            if self.m_nStatus == 0:
                return True, dTmpTime.value, dTmpStep.value
        except:
            return False, 0.0, 0.0
        return False, 0.0, 0.0

    def GetFifoDepth(self):
        try:
            return ISLLib.IOGetFifoDepth(self.__m_cData)
        except:
            return 0
        return 0

    def GetFifoReaders(self):
        try:
            return ISLLib.IOGetFifoReaders(self.__m_cData)
        except:
            return 0
        return 0

    def GetFifoIndWriter(self):
        try:
            return ISLLib.IOGetFifoIndWriter(self.__m_cData)
        except:
            return 0
        return 0

    def GetFifoIndReader(self, i):
        try:
            return ISLLib.IOGetFifoIndReader(self.__m_cData, i)
        except:
            return 0
        return 0

    def StoreData(self, dTime):
        try:
            self.m_nStatus = ISLLib.IOStoreData(self.__m_cData, self.m_cValue, dTime)
            if self.m_nStatus == 0 or self.m_nStatus == -3:
                return True
        except:
            return False
        return False

class CISLFloat(CISLData):
    """ISL Double Data"""

    def __init__(self, cParent):
        CISLData.__init__(self, cParent)
        self.m_dValue = c_double(0.0)
        self.m_cValue = pointer(self.m_dValue)
        self.m_nType = 0 # Real

    def __del__(self):
        CISLData.__del__(self)

    def GetValue(self):
        return self.m_dValue.value

    def SetValue(self, dValue):
        self.m_dValue = c_double(dValue)
        self.m_cValue = pointer(self.m_dValue)

class CISLInt(CISLData):
    """ISL Integer Data"""

    def __init__(self, cParent):
        CISLData.__init__(self, cParent)
        self.m_nValue = c_int(0)
        self.m_cValue = pointer(self.m_nValue)
        self.m_nType = 1 # Integer

    def __del__(self):
        CISLData.__del__(self)

    def GetValue(self):
        return self.m_nValue.value

    def SetValue(self, nValue):
        self.m_nValue = c_int(nValue)
        self.m_cValue = pointer(self.m_nValue)

class CISLBool(CISLData):
    """ISL Boolean Data"""

    def __init__(self, cParent):
        CISLData.__init__(self, cParent)
        self.m_bValue = c_bool(False)
        self.m_cValue = pointer(self.m_bValue)
        self.m_nType = 2 # Boolean

    def __del__(self):
        CISLData.__del__(self)

    def GetValue(self):
        return self.m_bValue.value

    def SetValue(self, bValue):
        self.m_bValue = c_bool(bValue)
        self.m_cValue = pointer(self.m_bValue)

class CISLString(CISLData):
    """ISL String Data"""

    def __init__(self, cParent):
        CISLData.__init__(self, cParent)
        self.m_sValue = c_char_p('')
        self.m_cValue = self.m_sValue
        self.m_nType = 3 # String

    def __del__(self):
        CISLData.__del__(self)

    def GetValue(self):
        return self.m_sValue.decode('utf-8')

    def SetValue(self, sValue):
        self.m_sValue = sValue.encode('utf-8')
        self.m_cValue = self.m_sValue

class CISLThreadRead(threading.Thread):
    """ISL Read Data in a thread"""

    def __init__(self, cData, aFunction, *args):
        threading.Thread.__init__(self)
        self.m_bStop = False
        self.m_bStatus = True
        self.__m_cData = cData
        self.__m_aFunction = aFunction
        self.__m_args = args

    def run(self):
        while not self.m_bStop:
            try:
                bRet, dReadTime = self.__m_cData.GetLastData(True)
                if not bRet:
                    self.m_bStop = True
                    self.m_bStatus = False
                else:
                    self.__m_aFunction(self.__m_cData, dReadTime, *self.__m_args)
                    self.m_bStop = self.__m_cData.IsTerminated()
            except:
                self.m_bStop = True
                self.m_bStatus = False
