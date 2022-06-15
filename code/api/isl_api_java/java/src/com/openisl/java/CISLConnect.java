package com.openisl.java;

public class CISLConnect {
	static {
		System.loadLibrary("isl_java");
	}
	
	private String m_sName;

	private native int Initialize(int nOwner);
	// Return 0 on success
	
	public native int New(String sName);
	// Return 0 on success
	public native int Load(String sFileName);
	// Return 0 on success
	public native int Save(String sFileName);
	// Return 0 on success
	public native int SetViewer(int nMode);
	// Return 0 on success
	public native int SetSessionId(String sSessionId);
	// Return 0 on success
	public native int Create(String sSessionId);
	// Return 0 on success
	public native int Check();
	// Return 0 on success
	public native int Connect(int nWait);
	// Return 0 on success
	public native int IsTerminated();
	// Return 0 if terminated, 1 if not and -1 if failed
	public native int Disconnect();
	// Return 0 on success
	public native void Free();

	public native int SetType(String sType);
	// Return 0 on success

	public native int OpenStore();
	// Return 0 on success
	public native int CloseStore();
	// Return 0 on success

	public native int SetMode(int nMode);
	// Return 0 on success
	public native int StartListenToExit();
	// Return 0 on success
	public native int ListenToExitSession();
	// Return 0 on success
	public native int SendStopRequest();
	// Return 0 on success
	public native int SendStopSession(String sSession);
	// Return 0 on success
	public native int SendStopMySession();
	// Return 0 on success
	
	public native int SetStartTime(double dVal);
	// Return 0 on success
	public native double GetStartTime();
	// Return -1.0 if failed
	public native int SetEndTime(double dVal);
	// Return 0 on success
	public native double GetEndTime();
	// Return -1.0 if failed
	public native int SetStepSize(double dVal);
	// Return 0 on success
	public native double GetStepSize();
	// Return -1.0 if failed

	public native int GetNbIOs();
	// Return a negative integer if failed
	public native int GetIOFromId(String sId);
	// Return a negative integer if failed
	public native int NewIO(String sId, int nCausality, int nType, int nSize);
	// Return a negative integer if failed
	
	public native void PrintError(int nId, String sMsg);
	public native void PrintWarning(int nId, String sMsg);
	public native void PrintInfo(int nId, String sMsg);
	
	public CISLConnect(int nOwner) {
		int nRet = Initialize(nOwner);
		if (nRet != 0) {
			System.err.println("ISL ERROR: Failed to initialize the connector. Error code: " + nRet);
		}
	}
}
