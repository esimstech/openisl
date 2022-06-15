package com.openisl.java;

public class CISLData {
	private int m_nInd;
	private String m_sId;
	
	private native int SetConnectionId(int nInd, String sId);

	public CISLData(CISLConnect cConnect, String sId) {
		m_sId = sId;
		m_nInd = cConnect.GetIOFromId(sId);
		if (m_nInd < 0) {
			System.err.println("ISL ERROR: Failed to get a variable from id: '" + sId + "'");
		}
	}
	
	public CISLData(CISLConnect cConnect, String sId, int nCausality, int nType, int nSize) {
		m_sId = sId;
		m_nInd = cConnect.NewIO(sId, nCausality, nType, nSize);
		if (m_nInd < 0) {
			System.err.println("ISL ERROR: Failed to create a variable with the id: '" + sId + "'");
		}
	}

	public boolean SetConnectId(String sId) {
		if (SetConnectionId(m_nInd, sId) < 0) {
			System.err.println("ISL ERROR: Failed to set the connection id '" + sId + "' on variable '" + m_sId + "'");
			return false;
		}
		return true;
	}

	public boolean IsValid() {
		return m_nInd >= 0;
	}
	
	public int GetInd() {
		return m_nInd;
	}
	
	public String GetId() {
		return m_sId;
	}
}
