/* 
 *     Name: CISLStringData.java
 *
 *     Description: Java OpenISL string variable.
 *
 *     Author: T. Roudier
 *     Copyright (c) 2019-2025 E-Sim Solutions Inc
 *
 *     Distributed under the MIT License.
 * 
 *     --------------------------------------------------------------------------
 * 
 *     Permission is hereby granted, free of charge, to any person obtaining a
 *     copy of this software and associated documentation files (the “Software”),
 *     to deal in the Software without restriction, including without limitation
 *     the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *     and/or sell copies of the Software, and to permit persons to whom the
 *     Software is furnished to do so, subject to the following conditions:
 *
 *     The above copyright notice and this permission notice shall be included in
 *     all copies or substantial portions of the Software.
 *
 *     THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *     THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *     FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 *     DEALINGS IN THE SOFTWARE.
 * 
 *     --------------------------------------------------------------------------
 *
 */

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
