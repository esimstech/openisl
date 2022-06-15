import sys

from ctypes import *
from openisl import CISLLib
from openisl.Singleton import *
from openisl.ISLUtils import *

ISLLib = CISLLib.CISLLib()

class CISLSims(object):
    """ISL Simulations"""

    def __init__(self):
        self.m_nType = 0
        self.m_nId = 0
        self.m_nPID = 0
        self.m_nNbIOs = 0
        self.m_sUUID = ''
        self.m_sName = ''
        self.m_sFile = ''
        self.m_sSessionId = ''
        self.m_nMaxSims = 0
        try:
            self.m_nMaxSims = ISLLib.SimsGetMaxNb()
        except:
            e = sys.exc_info()
            ISLLogError(4000, e[0], ": ", e[1])
            self.m_nMaxSims = 0

    def __del__(self):
        try:
            ISLLib.SimsClose()
        except:
            e = sys.exc_info()
            ISLLogError(4001, e[0], ": ", e[1])

    def GetSim(self, nInd):
        try:
            if ISLLib.SimsGet(nInd) != 0:
                return False
            self.m_nType = ISLLib.SimsGetType()
            self.m_nId = ISLLib.SimsGetId()
            self.m_nPID = ISLLib.SimsGetPID()
            self.m_nNbIOs = ISLLib.SimsGetNbIOs()
            self.m_sUUID = ISLLib.SimsGetUUID().decode('utf-8')
            self.m_sName = ISLLib.SimsGetName().decode('utf-8')
            self.m_sFile = ISLLib.SimsGetFile().decode('utf-8')
            self.m_sSessionId = ISLLib.SimsGetSessionId().decode('utf-8')
            return True
        except:
            e = sys.exc_info()
            ISLLogError(4002, e[0], ": ", e[1])
            return False
