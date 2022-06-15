package com.openisl.java;

public class CISLStringData extends CISLData {
	private String m_sData;
	private double[] m_dTimes;

	private native String GetString(int nInd, double[] dTimes, int nWait);
	private native void SetString(int nInd, String sVal, double dTime, int nWait);
	private native void SetStringAndStep(int nInd, String sVal, double dTime, double dStep, int nWait);
	private native void StoreString(int nInd, String sVal, double dTime);
	private native int InitString(int nInd, String sVal);
	
	public CISLStringData(CISLConnect cConnect, String sId) {
		super(cConnect, sId);
		m_dTimes = new double[2];
		m_dTimes[0] = 0.0;
		m_dTimes[1] = 0.0;
		m_sData = "";
	}
	
	public CISLStringData(CISLConnect cConnect, String sId, int nCausality, int nSize) {
		super(cConnect, sId, nCausality, 3, nSize);
		m_dTimes = new double[2];
		m_dTimes[0] = 0.0;
		m_dTimes[1] = 0.0;
		m_sData = "";
	}

	public boolean Initialize(String sVal) {
		if (InitString(GetInd(), sVal) < 0) {
			return false;
		}
		return true;
	}

	public String GetData(double dTime, int nWait) {
		m_dTimes[1] = dTime;
		m_sData = GetString(GetInd(), m_dTimes, nWait);
		StoreString(GetInd(), m_sData, m_dTimes[0]);
		return m_sData;
	}
	
	public double GetTime() {
		return m_dTimes[0];
	}

	public void SetData(String sVal, double dTime, int nWait) {
		m_dTimes[0] = dTime;
		m_dTimes[1] = dTime;
		m_sData = sVal;
		SetString(GetInd(), sVal, dTime, nWait);
		StoreString(GetInd(), sVal, dTime);
	}

	public void SetDataAndStep(String sVal, double dTime, double dStep, int nWait) {
		m_dTimes[0] = dTime;
		m_dTimes[1] = dTime;
		m_sData = sVal;
		SetStringAndStep(GetInd(), sVal, dTime, dStep, nWait);
		StoreString(GetInd(), sVal, dTime);
	}
}
