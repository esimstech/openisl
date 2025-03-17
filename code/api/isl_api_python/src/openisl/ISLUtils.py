# 
#     Name: CISLUtils.py
#
#     Description: Python OpenISL utilities and tools.
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
from openisl import CISLLib

ISLLib = CISLLib.CISLLib()

def ISLLogError(nId, *args):
    try:
        ISLLib.LLogError(nId, *args)
    except:
        sMsg = ''.join(str(i) for i in args)
        print("Error [", nId, "]: ", sMsg, sep='', file=sys.stderr)

def ISLLogWarning(nId, *args):
    try:
        ISLLib.LLogWarning(nId, *args)
    except:
        sMsg = ''.join(str(i) for i in args)
        print("Warning [", nId, "]: ", sMsg, sep='', file=sys.stderr)

def ISLLogInfo(nId, *args):
    try:
        ISLLib.LLogInfo(nId, *args)
    except:
        sMsg = ''.join(str(i) for i in args)
        print("Info [", nId, "]: ", sMsg, sep='')

def ISLLogDebug(nLevel, nId, *args):
    try:
        ISLLib.LLogDebug(nLevel, nId, *args)
    except:
        sMsg = ''.join(str(i) for i in args)
        print("Debug [", nId, "]: ", sMsg, sep='')

def ISLGetLogFile():
    try:
        sVal = ISLLib.GetLogFile().decode('utf-8')
        return sVal
    except:
        e = sys.exc_info()
        ISLLogError(1000, e[0], ": ", e[1])
        return ''
    return ''

def ISLGetVersion():
    try:
        sVal = ISLLib.GetVersion().decode('utf-8')
        return sVal
    except:
        e = sys.exc_info()
        ISLLogError(1001, e[0], ": ", e[1])
        return ''
    return ''

def ISLSendStopRequest():
    try:
        nRet = ISLLib.SendStopRequest()
        if nRet != 0:
            ISLLogError(1002, "ISLSendStopRequest Failed. Error code: ", nRet)
            return False
        return True
    except:
        e = sys.exc_info()
        ISLLogError(1003, e[0], ": ", e[1])
        return False
    return False

def ISLSendStopSession(sSession):
    try:
        nRet = ISLLib.SendStopSession(sSession.encode('utf-8'))
        if nRet != 0:
            ISLLogError(1004, "ISLSendStopSession Failed. Error code: ", nRet)
            return False
        return True
    except:
        e = sys.exc_info()
        ISLLogError(1005, e[0], ": ", e[1])
        return False
    return False
