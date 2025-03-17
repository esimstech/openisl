# 
#     Name: CISLSims.py
#
#     Description: Python OpenISL simulation management.
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
