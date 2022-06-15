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
