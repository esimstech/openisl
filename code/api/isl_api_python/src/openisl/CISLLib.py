# 
#     Name: CISLLib.py
#
#     Description: Python OpenISL Dynamic Library integration.
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

from __future__ import print_function
import os
import sys

from ctypes import *
from ctypes.util import find_library
from openisl.Singleton import *

class CISLLib(Singleton):
    """OpenISL Library Mapping"""

    def __init__(self):
        self.GetVersion = None

        self.GetLogFile = None
        self.LogError = None
        self.LogWarning = None
        self.LogInfo = None
        self.LogDebug = None

        self.StartThreadListenToStop = None
        self.SendStopRequest = None
        self.SendStopSession = None

        self.ConnectInit = None
        self.ConnectFree = None
        self.ConnectNew = None
        self.ConnectLoad = None
        self.ConnectCheck = None
        self.ConnectSave = None

        self.ConnectSetName = None
        self.ConnectGetName = None
        self.ConnectGetId = None
        self.ConnectGetUId = None
        self.ConnectSetType = None
        self.ConnectGetType = None
        self.ConnectGetTypeAsStr = None
        self.ConnectSetPID = None
        self.ConnectGetPID = None
        self.ConnectGetFileName = None

        self.ConnectSetSessionId = None
        self.ConnectGetSessionId = None
        self.ConnectSetConnectTimeOut = None
        self.ConnectGetConnectTimeOut = None
        self.ConnectSetStartTime = None
        self.ConnectGetStartTime = None
        self.ConnectSetEndTime = None
        self.ConnectGetEndTime = None
        self.ConnectSetStepSize = None
        self.ConnectGetStepSize = None
        self.ConnectSetStepTolerance = None
        self.ConnectGetStepTolerance = None

        self.ConnectNewIO = None
        self.ConnectRemoveIO = None

        self.ConnectGetNbIOs = None
        self.ConnectGetNbInputs = None
        self.ConnectGetNbOutputs = None

        self.ConnectIOExists = None
        self.ConnectGetIOFromStr = None
        self.ConnectGetIO = None
        self.ConnectGetInputFromStr = None
        self.ConnectGetInput = None
        self.ConnectGetOutputFromStr = None
        self.ConnectGetOutput = None

        self.ConnectCloseLogOnDelete = None
        self.ConnectSetMode = None
        self.ConnectGetMode = None
        self.ConnectSetViewer = None
        self.ConnectIsViewer = None

        self.ConnectSetTerminated = None
        self.ConnectIsTerminated = None

        self.ConnectCreate = None
        self.ConnectConnect = None
        self.ConnectDisconnect = None

        self.ConnectListenToExitSession = None
        self.ConnectSendStopSession = None

        self.IOGetId = None
        self.IOSetName = None
        self.IOGetName = None
        self.IOSetConnectId = None
        self.IOGetConnectId = None

        self.IOSetCausality = None
        self.IOGetCausality = None
        self.IOIsOutput = None
        self.IOIsInput = None

        self.IOGetTypeId = None
        self.IOGetType = None
        self.IOSetType = None

        self.TypeSetInitialAsStr = None
        self.TypeSetInitial = None
        self.TypeAllocate = None
        self.TypeFree = None
        self.TypeIsAllocated = None

        self.IOSetStepSize = None
        self.IOUseStore = None
        self.IOIsStoreUsed = None
        self.IOSetSyncTimeout = None
        self.IOGetSyncTimeout = None

        self.IOSetData = None
        self.IOSetDataAndNextStep = None
        self.IOGetLastData = None
        self.IOGetData = None

        self.IOSetEventData = None
        self.IOGetEventData = None

        self.IOSetDataAt = None
        self.IOGetDataAt = None

        self.IOGetMemData = None
        self.IOGetFifoDepth = None
        self.IOGetFifoReaders = None
        self.IOGetFifoIndWriter = None
        self.IOGetFifoIndReader = None

        self.IOStoreData = None

        self.SimsGetMaxNb = None
        self.SimsGet = None
        self.SimsClose = None

        self.SimsGetType = None
        self.SimsGetId = None
        self.SimsGetPID = None
        self.SimsGetNbIOs = None
        self.SimsGetUUID = None
        self.SimsGetName = None
        self.SimsGetFile = None
        self.SimsGetSessionId = None

        try:
            print("Info [L012]: Loading OpenISL dynamic library")
            if os.name == 'nt':
                self.m_Lib = cdll.LoadLibrary(find_library('isl_api.dll'))
            else:
                self.m_Lib = cdll.LoadLibrary('libisl_api.so')
            self.MapFunctions()
        except OSError as e:
            print("Error [L001][", e.errno, "]: ", e.strerror)

    def __del__(self):
        print("Info [L011]: Unmapping OpenISL functions")

        self.GetVersion = None

        self.GetLogFile = None
        self.LogError = None
        self.LogWarning = None
        self.LogInfo = None
        self.LogDebug = None

        self.StartThreadListenToStop = None
        self.SendStopRequest = None
        self.SendStopSession = None

        self.ConnectInit = None
        self.ConnectFree = None
        self.ConnectNew = None
        self.ConnectLoad = None
        self.ConnectCheck = None
        self.ConnectSave = None

        self.ConnectSetName = None
        self.ConnectGetName = None
        self.ConnectGetId = None
        self.ConnectGetUId = None
        self.ConnectSetType = None
        self.ConnectGetType = None
        self.ConnectGetTypeAsStr = None
        self.ConnectSetPID = None
        self.ConnectGetPID = None
        self.ConnectGetFileName = None

        self.ConnectSetSessionId = None
        self.ConnectGetSessionId = None
        self.ConnectSetConnectTimeOut = None
        self.ConnectGetConnectTimeOut = None
        self.ConnectSetStartTime = None
        self.ConnectGetStartTime = None
        self.ConnectSetEndTime = None
        self.ConnectGetEndTime = None
        self.ConnectSetStepSize = None
        self.ConnectGetStepSize = None
        self.ConnectSetStepTolerance = None
        self.ConnectGetStepTolerance = None

        self.ConnectNewIO = None
        self.ConnectRemoveIO = None

        self.ConnectGetNbIOs = None
        self.ConnectGetNbInputs = None
        self.ConnectGetNbOutputs = None

        self.ConnectIOExists = None
        self.ConnectGetIOFromStr = None
        self.ConnectGetIO = None
        self.ConnectGetInputFromStr = None
        self.ConnectGetInput = None
        self.ConnectGetOutputFromStr = None
        self.ConnectGetOutput = None

        self.ConnectCloseLogOnDelete = None
        self.ConnectSetMode = None
        self.ConnectGetMode = None
        self.ConnectSetViewer = None
        self.ConnectIsViewer = None

        self.ConnectSetTerminated = None
        self.ConnectIsTerminated = None

        self.ConnectCreate = None
        self.ConnectConnect = None
        self.ConnectDisconnect = None

        self.ConnectListenToExitSession = None
        self.ConnectSendStopSession = None

        self.IOGetId = None
        self.IOSetName = None
        self.IOGetName = None
        self.IOSetConnectId = None
        self.IOGetConnectId = None

        self.IOSetCausality = None
        self.IOGetCausality = None
        self.IOIsOutput = None
        self.IOIsInput = None

        self.IOGetTypeId = None
        self.IOGetType = None
        self.IOSetType = None

        self.TypeSetInitialAsStr = None
        self.TypeSetInitial = None
        self.TypeAllocate = None
        self.TypeFree = None
        self.TypeIsAllocated = None

        self.IOSetStepSize = None
        self.IOUseStore = None
        self.IOIsStoreUsed = None
        self.IOSetSyncTimeout = None
        self.IOGetSyncTimeout = None

        self.IOSetData = None
        self.IOSetDataAndNextStep = None
        self.IOGetLastData = None
        self.IOGetData = None

        self.IOSetEventData = None
        self.IOGetEventData = None

        self.IOSetDataAt = None
        self.IOGetDataAt = None

        self.IOGetMemData = None
        self.IOGetFifoDepth = None
        self.IOGetFifoReaders = None
        self.IOGetFifoIndWriter = None
        self.IOGetFifoIndReader = None

        self.IOStoreData = None

        self.SimsGetMaxNb = None
        self.SimsGet = None
        self.SimsClose = None

        self.SimsGetType = None
        self.SimsGetId = None
        self.SimsGetPID = None
        self.SimsGetNbIOs = None
        self.SimsGetUUID = None
        self.SimsGetName = None
        self.SimsGetFile = None
        self.SimsGetSessionId = None

    def MapFunctions(self):
        print("Info [L013]: Mapping OpenISL functions")
        # ISL_GetVersionNumber
        try:
            self.GetVersion = self.m_Lib.ISL_GetVersionNumber
            self.GetVersion.restype = c_char_p
            self.GetVersion.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L061]: ", e[0], ": ", e[1])

        # ISL_GetLogFile
        try:
            self.GetLogFile = self.m_Lib.ISL_GetLogFile
            self.GetLogFile.restype = c_char_p
            self.GetLogFile.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L112]: ", e[0], ": ", e[1])

        # ISL_LogError
        try:
            self.LogError = self.m_Lib.ISL_LogError
            self.LogError.restype = None
        except:
            e = sys.exc_info()
            print("Error [L008]: ", e[0], ": ", e[1])

        # ISL_LogWarning
        try:
            self.LogWarning = self.m_Lib.ISL_LogWarning
            self.LogWarning.restype = None
        except:
            e = sys.exc_info()
            print("Error [L009]: ", e[0], ": ", e[1])

        # ISL_LogInfo
        try:
            self.LogInfo = self.m_Lib.ISL_LogInfo
            self.LogInfo.restype = None
        except:
            e = sys.exc_info()
            print("Error [L010]: ", e[0], ": ", e[1])

        # ISL_LogDebug
        try:
            self.LogDebug = self.m_Lib.ISL_LogDebug
            self.LogDebug.restype = None
        except:
            e = sys.exc_info()
            print("Error [L006]: ", e[0], ": ", e[1])

        # ISL_StartThreadListenToStop
        try:
            self.StartThreadListenToStop = self.m_Lib.ISL_StartThreadListenToStop
            self.StartThreadListenToStop.restype = c_int
            self.StartThreadListenToStop.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L029]: ", e[0], ": ", e[1])

        # ISL_SendStopRequest
        try:
            self.SendStopRequest = self.m_Lib.ISL_SendStopRequest
            self.SendStopRequest.restype = c_int
            self.SendStopRequest.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L030]: ", e[0], ": ", e[1])

        # ISL_SendStopSession
        try:
            self.SendStopSession = self.m_Lib.ISL_SendStopSession
            self.SendStopSession.restype = c_int
            self.SendStopSession.argtypes = [c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L046]: ", e[0], ": ", e[1])

        # ISL_ConnectInit
        try:
            self.ConnectInit = self.m_Lib.ISL_ConnectInit
            self.ConnectInit.restype = c_void_p
            self.ConnectInit.argtypes = [c_int]
        except:
            e = sys.exc_info()
            print("Error [L002]: ", e[0], ": ", e[1])

        # ISL_ConnectFree
        try:
            self.ConnectFree = self.m_Lib.ISL_ConnectFree
            self.ConnectFree.restype = c_int
            self.ConnectFree.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L003]: ", e[0], ": ", e[1])

        # ISL_ConnectNew
        try:
            self.ConnectNew = self.m_Lib.ISL_ConnectNew
            self.ConnectNew.restype = c_int
            self.ConnectNew.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L074]: ", e[0], ": ", e[1])

        # ISL_ConnectLoad
        try:
            self.ConnectLoad = self.m_Lib.ISL_ConnectLoad
            self.ConnectLoad.restype = c_int
            self.ConnectLoad.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L004]: ", e[0], ": ", e[1])

        # ISL_ConnectCheck
        try:
            self.ConnectCheck = self.m_Lib.ISL_ConnectCheck
            self.ConnectCheck.restype = c_int
            self.ConnectCheck.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L055]: ", e[0], ": ", e[1])

        # ISL_ConnectSave
        try:
            self.ConnectSave = self.m_Lib.ISL_ConnectSave
            self.ConnectSave.restype = c_int
            self.ConnectSave.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L056]: ", e[0], ": ", e[1])

        # ISL_ConnectSetName
        try:
            self.ConnectSetName = self.m_Lib.ISL_ConnectSetName
            self.ConnectSetName.restype = c_int
            self.ConnectSetName.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L075]: ", e[0], ": ", e[1])

        # ISL_ConnectGetName
        try:
            self.ConnectGetName = self.m_Lib.ISL_ConnectGetName
            self.ConnectGetName.restype = c_char_p
            self.ConnectGetName.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L076]: ", e[0], ": ", e[1])

        # ISL_ConnectGetId
        try:
            self.ConnectGetId = self.m_Lib.ISL_ConnectGetId
            self.ConnectGetId.restype = c_char_p
            self.ConnectGetId.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L077]: ", e[0], ": ", e[1])

        # ISL_ConnectGetUId
        try:
            self.ConnectGetUId = self.m_Lib.ISL_ConnectGetUId
            self.ConnectGetUId.restype = c_uint
            self.ConnectGetUId.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L078]: ", e[0], ": ", e[1])

        # ISL_ConnectSetType
        try:
            self.ConnectSetType = self.m_Lib.ISL_ConnectSetType
            self.ConnectSetType.restype = c_int
            self.ConnectSetType.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L079]: ", e[0], ": ", e[1])

        # ISL_ConnectGetType
        try:
            self.ConnectGetType = self.m_Lib.ISL_ConnectGetType
            self.ConnectGetType.restype = c_uint
            self.ConnectGetType.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L080]: ", e[0], ": ", e[1])

        # ISL_ConnectGetTypeAsStr
        try:
            self.ConnectGetTypeAsStr = self.m_Lib.ISL_ConnectGetTypeAsStr
            self.ConnectGetTypeAsStr.restype = c_char_p
            self.ConnectGetTypeAsStr.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L081]: ", e[0], ": ", e[1])

        # ISL_ConnectSetPID
        try:
            self.ConnectSetPID = self.m_Lib.ISL_ConnectSetPID
            self.ConnectSetPID.restype = c_int
            self.ConnectSetPID.argtypes = [c_void_p, c_ulong]
        except:
            e = sys.exc_info()
            print("Error [L082]: ", e[0], ": ", e[1])

        # ISL_ConnectGetPID
        try:
            self.ConnectGetPID = self.m_Lib.ISL_ConnectGetPID
            self.ConnectGetPID.restype = c_ulong
            self.ConnectGetPID.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L083]: ", e[0], ": ", e[1])

        # ISL_ConnectGetFileName
        try:
            self.ConnectGetFileName = self.m_Lib.ISL_ConnectGetFileName
            self.ConnectGetFileName.restype = c_char_p
            self.ConnectGetFileName.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L084]: ", e[0], ": ", e[1])

        # ISL_ConnectSetSessionId
        try:
            self.ConnectSetSessionId = self.m_Lib.ISL_ConnectSetSessionId
            self.ConnectSetSessionId.restype = c_int
            self.ConnectSetSessionId.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L069]: ", e[0], ": ", e[1])

        # ISL_ConnectGetSessionId
        try:
            self.ConnectGetSessionId = self.m_Lib.ISL_ConnectGetSessionId
            self.ConnectGetSessionId.restype = c_char_p
            self.ConnectGetSessionId.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L085]: ", e[0], ": ", e[1])

        # ISL_ConnectSetConnectTimeOut
        try:
            self.ConnectSetConnectTimeOut = self.m_Lib.ISL_ConnectSetConnectTimeOut
            self.ConnectSetConnectTimeOut.restype = c_int
            self.ConnectSetConnectTimeOut.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L086]: ", e[0], ": ", e[1])

        # ISL_ConnectGetConnectTimeOut
        try:
            self.ConnectGetConnectTimeOut = self.m_Lib.ISL_ConnectGetConnectTimeOut
            self.ConnectGetConnectTimeOut.restype = c_int
            self.ConnectGetConnectTimeOut.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L087]: ", e[0], ": ", e[1])

        # ISL_ConnectSetStartTime
        try:
            self.ConnectSetStartTime = self.m_Lib.ISL_ConnectSetStartTime
            self.ConnectSetStartTime.restype = c_int
            self.ConnectSetStartTime.argtypes = [c_void_p, c_double]
        except:
            e = sys.exc_info()
            print("Error [L088]: ", e[0], ": ", e[1])

        # ISL_ConnectGetStartTime
        try:
            self.ConnectGetStartTime = self.m_Lib.ISL_ConnectGetStartTime
            self.ConnectGetStartTime.restype = c_double
            self.ConnectGetStartTime.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L021]: ", e[0], ": ", e[1])

        # ISL_ConnectSetEndTime
        try:
            self.ConnectSetEndTime = self.m_Lib.ISL_ConnectSetEndTime
            self.ConnectSetEndTime.restype = c_int
            self.ConnectSetEndTime.argtypes = [c_void_p, c_double]
        except:
            e = sys.exc_info()
            print("Error [L089]: ", e[0], ": ", e[1])

        # ISL_ConnectGetEndTime
        try:
            self.ConnectGetEndTime = self.m_Lib.ISL_ConnectGetEndTime
            self.ConnectGetEndTime.restype = c_double
            self.ConnectGetEndTime.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L022]: ", e[0], ": ", e[1])

        # ISL_ConnectSetStepSize
        try:
            self.ConnectSetStepSize = self.m_Lib.ISL_ConnectSetStepSize
            self.ConnectSetStepSize.restype = c_int
            self.ConnectSetStepSize.argtypes = [c_void_p, c_double]
        except:
            e = sys.exc_info()
            print("Error [L090]: ", e[0], ": ", e[1])

        # ISL_ConnectGetStepSize
        try:
            self.ConnectGetStepSize = self.m_Lib.ISL_ConnectGetStepSize
            self.ConnectGetStepSize.restype = c_double
            self.ConnectGetStepSize.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L023]: ", e[0], ": ", e[1])
            
        # ISL_ConnectSetStepTolerance
        try:
            self.ConnectSetStepTolerance = self.m_Lib.ISL_ConnectSetStepTolerance
            self.ConnectSetStepTolerance.restype = c_int
            self.ConnectSetStepTolerance.argtypes = [c_void_p, c_double]
        except:
            e = sys.exc_info()
            print("Error [L091]: ", e[0], ": ", e[1])

        # ISL_ConnectGetStepTolerance
        try:
            self.ConnectGetStepTolerance = self.m_Lib.ISL_ConnectGetStepTolerance
            self.ConnectGetStepTolerance.restype = c_double
            self.ConnectGetStepTolerance.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L092]: ", e[0], ": ", e[1])

        # ISL_ConnectNewIO
        try:
            self.ConnectNewIO = self.m_Lib.ISL_ConnectNewIO
            self.ConnectNewIO.restype = c_void_p
            self.ConnectNewIO.argtypes = [c_void_p, c_char_p, c_int, c_int, c_int]
        except:
            e = sys.exc_info()
            print("Error [L093]: ", e[0], ": ", e[1])

        # ISL_ConnectRemoveIO
        try:
            self.ConnectRemoveIO = self.m_Lib.ISL_ConnectRemoveIO
            self.ConnectRemoveIO.restype = c_int
            self.ConnectRemoveIO.argtypes = [c_void_p, c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L094]: ", e[0], ": ", e[1])

        # ISL_ConnectGetNbIOs
        try:
            self.ConnectGetNbIOs = self.m_Lib.ISL_ConnectGetNbIOs
            self.ConnectGetNbIOs.restype = c_int
            self.ConnectGetNbIOs.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L014]: ", e[0], ": ", e[1])

        # ISL_ConnectGetNbInputs
        try:
            self.ConnectGetNbInputs = self.m_Lib.ISL_ConnectGetNbInputs
            self.ConnectGetNbInputs.restype = c_int
            self.ConnectGetNbInputs.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L015]: ", e[0], ": ", e[1])

        # ISL_ConnectGetNbOutputs
        try:
            self.ConnectGetNbOutputs = self.m_Lib.ISL_ConnectGetNbOutputs
            self.ConnectGetNbOutputs.restype = c_int
            self.ConnectGetNbOutputs.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L016]: ", e[0], ": ", e[1])

        # ISL_ConnectIOExists
        try:
            self.ConnectIOExists = self.m_Lib.ISL_ConnectIOExists
            self.ConnectIOExists.restype = c_int
            self.ConnectIOExists.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L095]: ", e[0], ": ", e[1])

        # ISL_ConnectGetIOFromStr
        try:
            self.ConnectGetIOFromStr = self.m_Lib.ISL_ConnectGetIOFromStr
            self.ConnectGetIOFromStr.restype = c_void_p
            self.ConnectGetIOFromStr.argtypes = [c_void_p, c_char_p, POINTER(c_int)]
        except:
            e = sys.exc_info()
            print("Error [L017]: ", e[0], ": ", e[1])

        # ISL_ConnectGetIO
        try:
            self.ConnectGetIO = self.m_Lib.ISL_ConnectGetIO
            self.ConnectGetIO.restype = c_void_p
            self.ConnectGetIO.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L018]: ", e[0], ": ", e[1])

        # ISL_ConnectGetInputFromStr
        try:
            self.ConnectGetInputFromStr = self.m_Lib.ISL_ConnectGetInputFromStr
            self.ConnectGetInputFromStr.restype = c_void_p
            self.ConnectGetInputFromStr.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L096]: ", e[0], ": ", e[1])

        # ISL_ConnectGetInput
        try:
            self.ConnectGetInput = self.m_Lib.ISL_ConnectGetInput
            self.ConnectGetInput.restype = c_void_p
            self.ConnectGetInput.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L019]: ", e[0], ": ", e[1])

        # ISL_ConnectGetOutputFromStr
        try:
            self.ConnectGetOutputFromStr = self.m_Lib.ISL_ConnectGetOutputFromStr
            self.ConnectGetOutputFromStr.restype = c_void_p
            self.ConnectGetOutputFromStr.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L097]: ", e[0], ": ", e[1])

        # ISL_ConnectGetOutput
        try:
            self.ConnectGetOutput = self.m_Lib.ISL_ConnectGetOutput
            self.ConnectGetOutput.restype = c_void_p
            self.ConnectGetOutput.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L020]: ", e[0], ": ", e[1])

        # ISL_ConnectCloseLogOnDelete
        try:
            self.ConnectCloseLogOnDelete = self.m_Lib.ISL_ConnectCloseLogOnDelete
            self.ConnectCloseLogOnDelete.restype = c_int
            self.ConnectCloseLogOnDelete.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L005]: ", e[0], ": ", e[1])

        # ISL_ConnectSetMode
        try:
            self.ConnectSetMode = self.m_Lib.ISL_ConnectSetMode
            self.ConnectSetMode.restype = c_int
            self.ConnectSetMode.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L028]: ", e[0], ": ", e[1])

        # ISL_ConnectGetMode
        try:
            self.ConnectGetMode = self.m_Lib.ISL_ConnectGetMode
            self.ConnectGetMode.restype = c_int
            self.ConnectGetMode.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L098]: ", e[0], ": ", e[1])

        # ISL_ConnectSetViewer
        try:
            self.ConnectSetViewer = self.m_Lib.ISL_ConnectSetViewer
            self.ConnectSetViewer.restype = c_int
            self.ConnectSetViewer.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L099]: ", e[0], ": ", e[1])

        # ISL_ConnectIsViewer
        try:
            self.ConnectIsViewer = self.m_Lib.ISL_ConnectIsViewer
            self.ConnectIsViewer.restype = c_int
            self.ConnectIsViewer.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L100]: ", e[0], ": ", e[1])

        # ISL_ConnectSetTerminated
        try:
            self.ConnectSetTerminated = self.m_Lib.ISL_ConnectSetTerminated
            self.ConnectSetTerminated.restype = c_int
            self.ConnectSetTerminated.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L062]: ", e[0], ": ", e[1])

        # ISL_ConnectIsTerminated
        try:
            self.ConnectIsTerminated = self.m_Lib.ISL_ConnectIsTerminated
            self.ConnectIsTerminated.restype = c_int
            self.ConnectIsTerminated.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L026]: ", e[0], ": ", e[1])

        # ISL_ConnectCreate
        try:
            self.ConnectCreate = self.m_Lib.ISL_ConnectCreate
            self.ConnectCreate.restype = c_int
            self.ConnectCreate.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L024]: ", e[0], ": ", e[1])

        # ISL_ConnectConnect
        try:
            self.ConnectConnect = self.m_Lib.ISL_ConnectConnect
            self.ConnectConnect.restype = c_int
            self.ConnectConnect.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L025]: ", e[0], ": ", e[1])

        # ISL_ConnectDisconnect
        try:
            self.ConnectDisconnect = self.m_Lib.ISL_ConnectDisconnect
            self.ConnectDisconnect.restype = c_int
            self.ConnectDisconnect.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L027]: ", e[0], ": ", e[1])

        # ISL_ConnectListenToExitSession
        try:
            self.ConnectListenToExitSession = self.m_Lib.ISL_ConnectListenToExitSession
            self.ConnectListenToExitSession.restype = c_int
            self.ConnectListenToExitSession.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L045]: ", e[0], ": ", e[1])

        # ISL_ConnectSendStopSession
        try:
            self.ConnectSendStopSession = self.m_Lib.ISL_ConnectSendStopSession
            self.ConnectSendStopSession.restype = c_int
            self.ConnectSendStopSession.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L047]: ", e[0], ": ", e[1])

        # ISL_IOGetId
        try:
            self.IOGetId = self.m_Lib.ISL_IOGetId
            self.IOGetId.restype = c_char_p
            self.IOGetId.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L063]: ", e[0], ": ", e[1])

        # ISL_IOSetName
        try:
            self.IOSetName = self.m_Lib.ISL_IOSetName
            self.IOSetName.restype = c_int
            self.IOSetName.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L064]: ", e[0], ": ", e[1])

        # ISL_IOGetName
        try:
            self.IOGetName = self.m_Lib.ISL_IOGetName
            self.IOGetName.restype = c_char_p
            self.IOGetName.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L033]: ", e[0], ": ", e[1])

        # ISL_IOSetConnectId
        try:
            self.IOSetConnectId = self.m_Lib.ISL_IOSetConnectId
            self.IOSetConnectId.restype = c_int
            self.IOSetConnectId.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L031]: ", e[0], ": ", e[1])

        # ISL_IOGetConnectId
        try:
            self.IOGetConnectId = self.m_Lib.ISL_IOGetConnectId
            self.IOGetConnectId.restype = c_char_p
            self.IOGetConnectId.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L032]: ", e[0], ": ", e[1])

        # ISL_IOSetCausality
        try:
            self.IOSetCausality = self.m_Lib.ISL_IOSetCausality
            self.IOSetCausality.restype = c_int
            self.IOSetCausality.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L071]: ", e[0], ": ", e[1])

        # ISL_IOGetCausality
        try:
            self.IOGetCausality = self.m_Lib.ISL_IOGetCausality
            self.IOGetCausality.restype = c_int
            self.IOGetCausality.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L044]: ", e[0], ": ", e[1])

        # ISL_IOIsOutput
        try:
            self.IOIsOutput = self.m_Lib.ISL_IOIsOutput
            self.IOIsOutput.restype = c_int
            self.IOIsOutput.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L034]: ", e[0], ": ", e[1])

        # ISL_IOIsInput
        try:
            self.IOIsInput = self.m_Lib.ISL_IOIsInput
            self.IOIsInput.restype = c_int
            self.IOIsInput.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L035]: ", e[0], ": ", e[1])

        # ISL_IOGetTypeId
        try:
            self.IOGetTypeId = self.m_Lib.ISL_IOGetTypeId
            self.IOGetTypeId.restype = c_int
            self.IOGetTypeId.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L036]: ", e[0], ": ", e[1])

        # ISL_IOGetType
        try:
            self.IOGetType = self.m_Lib.ISL_IOGetType
            self.IOGetType.restype = c_void_p
            self.IOGetType.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L101]: ", e[0], ": ", e[1])

        # ISL_IOSetType
        try:
            self.IOSetType = self.m_Lib.ISL_IOSetType
            self.IOSetType.restype = c_int
            self.IOSetType.argtypes = [c_void_p, c_int, c_int]
        except:
            e = sys.exc_info()
            print("Error [L102]: ", e[0], ": ", e[1])

        # ISL_TypeSetInitialAsStr
        try:
            self.TypeSetInitialAsStr = self.m_Lib.ISL_TypeSetInitialAsStr
            self.TypeSetInitialAsStr.restype = c_int
            self.TypeSetInitialAsStr.argtypes = [c_void_p, c_char_p]
        except:
            e = sys.exc_info()
            print("Error [L103]: ", e[0], ": ", e[1])

        # ISL_TypeSetInitial
        try:
            self.TypeSetInitial = self.m_Lib.ISL_TypeSetInitial
            self.TypeSetInitial.restype = c_int
            self.TypeSetInitial.argtypes = [c_void_p, c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L104]: ", e[0], ": ", e[1])

        # ISL_TypeAllocate
        try:
            self.TypeAllocate = self.m_Lib.ISL_TypeAllocate
            self.TypeAllocate.restype = c_int
            self.TypeAllocate.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L105]: ", e[0], ": ", e[1])

        # ISL_TypeFree
        try:
            self.TypeFree = self.m_Lib.ISL_TypeFree
            self.TypeFree.restype = c_int
            self.TypeFree.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L106]: ", e[0], ": ", e[1])

        # ISL_TypeIsAllocated
        try:
            self.TypeIsAllocated = self.m_Lib.ISL_TypeIsAllocated
            self.TypeIsAllocated.restype = c_int
            self.TypeIsAllocated.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L107]: ", e[0], ": ", e[1])

        # ISL_IOSetStepSize
        try:
            self.IOSetStepSize = self.m_Lib.ISL_IOSetStepSize
            self.IOSetStepSize.restype = c_int
            self.IOSetStepSize.argtypes = [c_void_p, c_double]
        except:
            e = sys.exc_info()
            print("Error [L108]: ", e[0], ": ", e[1])

        # ISL_IOUseStore
        try:
            self.IOUseStore = self.m_Lib.ISL_IOUseStore
            self.IOUseStore.restype = c_int
            self.IOUseStore.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L109]: ", e[0], ": ", e[1])

        # ISL_IOIsStoreUsed
        try:
            self.IOIsStoreUsed = self.m_Lib.ISL_IOIsStoreUsed
            self.IOIsStoreUsed.restype = c_int
            self.IOIsStoreUsed.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L110]: ", e[0], ": ", e[1])

        # ISL_IOSetSyncTimeout
        try:
            self.IOSetSyncTimeout = self.m_Lib.ISL_IOSetSyncTimeout
            self.IOSetSyncTimeout.restype = c_int
            self.IOSetSyncTimeout.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L111]: ", e[0], ": ", e[1])

        # ISL_IOGetSyncTimeout
        try:
            self.IOGetSyncTimeout = self.m_Lib.ISL_IOGetSyncTimeout
            self.IOGetSyncTimeout.restype = c_int
            self.IOGetSyncTimeout.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L007]: ", e[0], ": ", e[1])

        # ISL_IOSetData
        try:
            self.IOSetData = self.m_Lib.ISL_IOSetData
            self.IOSetData.restype = c_int
            self.IOSetData.argtypes = [c_void_p, c_void_p, c_double, c_int]
        except:
            e = sys.exc_info()
            print("Error [L037]: ", e[0], ": ", e[1])

        # ISL_IOSetDataAndNextStep
        try:
            self.IOSetDataAndNextStep = self.m_Lib.ISL_IOSetDataAndNextStep
            self.IOSetDataAndNextStep.restype = c_int
            self.IOSetDataAndNextStep.argtypes = [c_void_p, c_void_p, c_double, c_double, c_int]
        except:
            e = sys.exc_info()
            print("Error [L038]: ", e[0], ": ", e[1])

        # ISL_IOGetLastData
        try:
            self.IOGetLastData = self.m_Lib.ISL_IOGetLastData
            self.IOGetLastData.restype = c_int
            self.IOGetLastData.argtypes = [c_void_p, c_void_p, POINTER(c_double), c_int]
        except:
            e = sys.exc_info()
            print("Error [L039]: ", e[0], ": ", e[1])

        # ISL_IOGetData
        try:
            self.IOGetData = self.m_Lib.ISL_IOGetData
            self.IOGetData.restype = c_int
            self.IOGetData.argtypes = [c_void_p, c_void_p, POINTER(c_double), c_double, c_int]
        except:
            e = sys.exc_info()
            print("Error [L040]: ", e[0], ": ", e[1])

        # ISL_IOSetEventData
        try:
            self.IOSetEventData = self.m_Lib.ISL_IOSetEventData
            self.IOSetEventData.restype = c_int
            self.IOSetEventData.argtypes = [c_void_p, c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L072]: ", e[0], ": ", e[1])

        # ISL_IOGetEventData
        try:
            self.IOGetEventData = self.m_Lib.ISL_IOGetEventData
            self.IOGetEventData.restype = c_int
            self.IOGetEventData.argtypes = [c_void_p, c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L073]: ", e[0], ": ", e[1])

        # ISL_IOSetDataAt
        try:
            self.IOSetDataAt = self.m_Lib.ISL_IOSetDataAt
            self.IOSetDataAt.restype = c_int
            self.IOSetDataAt.argtypes = [c_void_p, c_void_p, c_double, c_int]
        except:
            e = sys.exc_info()
            print("Error [L041]: ", e[0], ": ", e[1])

        # ISL_IOGetDataAt
        try:
            self.IOGetDataAt = self.m_Lib.ISL_IOGetDataAt
            self.IOGetDataAt.restype = c_int
            self.IOGetDataAt.argtypes = [c_void_p, c_void_p, POINTER(c_double), c_int]
        except:
            e = sys.exc_info()
            print("Error [L042]: ", e[0], ": ", e[1])

        # ISL_IOGetMemData
        try:
            self.IOGetMemData = self.m_Lib.ISL_IOGetMemData
            self.IOGetMemData.restype = c_int
            self.IOGetMemData.argtypes = [c_void_p, c_void_p, POINTER(c_double), POINTER(c_double), c_int]
        except:
            e = sys.exc_info()
            print("Error [L065]: ", e[0], ": ", e[1])

        # ISL_IOGetFifoDepth
        try:
            self.IOGetFifoDepth = self.m_Lib.ISL_IOGetFifoDepth
            self.IOGetFifoDepth.restype = c_int
            self.IOGetFifoDepth.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L066]: ", e[0], ": ", e[1])

        # ISL_IOGetFifoReaders
        try:
            self.IOGetFifoReaders = self.m_Lib.ISL_IOGetFifoReaders
            self.IOGetFifoReaders.restype = c_int
            self.IOGetFifoReaders.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L070]: ", e[0], ": ", e[1])

        # ISL_IOGetFifoIndWriter
        try:
            self.IOGetFifoIndWriter = self.m_Lib.ISL_IOGetFifoIndWriter
            self.IOGetFifoIndWriter.restype = c_int
            self.IOGetFifoIndWriter.argtypes = [c_void_p]
        except:
            e = sys.exc_info()
            print("Error [L067]: ", e[0], ": ", e[1])

        # ISL_IOGetFifoIndReader
        try:
            self.IOGetFifoIndReader = self.m_Lib.ISL_IOGetFifoIndReader
            self.IOGetFifoIndReader.restype = c_int
            self.IOGetFifoIndReader.argtypes = [c_void_p, c_int]
        except:
            e = sys.exc_info()
            print("Error [L068]: ", e[0], ": ", e[1])

        # ISL_IOStoreData
        try:
            self.IOStoreData = self.m_Lib.ISL_IOStoreData
            self.IOStoreData.restype = c_int
            self.IOStoreData.argtypes = [c_void_p, c_void_p, c_double]
        except:
            e = sys.exc_info()
            print("Error [L043]: ", e[0], ": ", e[1])

        # ISL_SimsGetMaxNb
        try:
            self.SimsGetMaxNb = self.m_Lib.ISL_SimsGetMaxNb
            self.SimsGetMaxNb.restype = c_int
            self.SimsGetMaxNb.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L048]: ", e[0], ": ", e[1])

        # ISL_SimsGet
        try:
            self.SimsGet = self.m_Lib.ISL_SimsGet
            self.SimsGet.restype = c_int
            self.SimsGet.argtypes = [c_int]
        except:
            e = sys.exc_info()
            print("Error [L049]: ", e[0], ": ", e[1])

        # ISL_SimsClose
        try:
            self.SimsClose = self.m_Lib.ISL_SimsClose
            self.SimsClose.restype = c_int
            self.SimsClose.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L050]: ", e[0], ": ", e[1])

        # ISL_SimsGetType
        try:
            self.SimsGetType = self.m_Lib.ISL_SimsGetType
            self.SimsGetType.restype = c_int
            self.SimsGetType.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L057]: ", e[0], ": ", e[1])

        # ISL_SimsGetId
        try:
            self.SimsGetId = self.m_Lib.ISL_SimsGetId
            self.SimsGetId.restype = c_uint
            self.SimsGetId.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L051]: ", e[0], ": ", e[1])

        # ISL_SimsGetPID
        try:
            self.SimsGetPID = self.m_Lib.ISL_SimsGetPID
            self.SimsGetPID.restype = c_ulong
            self.SimsGetPID.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L052]: ", e[0], ": ", e[1])

        # ISL_SimsGetNbIOs
        try:
            self.SimsGetNbIOs = self.m_Lib.ISL_SimsGetNbIOs
            self.SimsGetNbIOs.restype = c_int
            self.SimsGetNbIOs.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L058]: ", e[0], ": ", e[1])

        # ISL_SimsGetUUID
        try:
            self.SimsGetUUID = self.m_Lib.ISL_SimsGetUUID
            self.SimsGetUUID.restype = c_char_p
            self.SimsGetUUID.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L059]: ", e[0], ": ", e[1])

        # ISL_SimsGetName
        try:
            self.SimsGetName = self.m_Lib.ISL_SimsGetName
            self.SimsGetName.restype = c_char_p
            self.SimsGetName.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L053]: ", e[0], ": ", e[1])

        # ISL_SimsGetFile
        try:
            self.SimsGetFile = self.m_Lib.ISL_SimsGetFile
            self.SimsGetFile.restype = c_char_p
            self.SimsGetFile.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L060]: ", e[0], ": ", e[1])

        # ISL_SimsGetSessionId
        try:
            self.SimsGetSessionId = self.m_Lib.ISL_SimsGetSessionId
            self.SimsGetSessionId.restype = c_char_p
            self.SimsGetSessionId.argtypes = []
        except:
            e = sys.exc_info()
            print("Error [L054]: ", e[0], ": ", e[1])

    def LLogError(self, nId, *args):
        sMsg = ''.join(str(i) for i in args)
        try:
            self.PrintError(nId, sMsg.encode('utf-8'))
        except:
            print("Error [", nId, "]: ", sMsg, sep='', file=sys.stderr)

    def LLogWarning(self, nId, *args):
        sMsg = ''.join(str(i) for i in args)
        try:
            self.LogWarning(nId, sMsg.encode('utf-8'))
        except:
            print("Warning [", nId, "]: ", sMsg, sep='', file=sys.stderr)

    def LLogInfo(self, nId, *args):
        sMsg = ''.join(str(i) for i in args)
        try:
            self.LogInfo(nId, sMsg.encode('utf-8'))
        except:
            print("Info [", nId, "]: ", sMsg, sep='')

    def LLogDebug(self, nLevel, nId, *args):
        sMsg = ''.join(str(i) for i in args)
        try:
            self.LogDebug(nLevel, nId, sMsg.encode('utf-8'))
        except:
            print("Debug [", nId, "]: ", sMsg, sep='')
