/* 
 *     Name: CISLIntData.java
 *
 *     Description: Java OpenISL integer variable.
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

public class CISLIntData extends CISLData {
	private int m_nData;
	private double[] m_dTimes;

	private native int GetInt(int nInd, double[] dTimes, int nWait);
	private native void SetInt(int nInd, int nVal, double dTime, int nWait);
	private native void SetIntAndStep(int nInd, int nVal, double dTime, double dStep, int nWait);
	private native void StoreInt(int nInd, int nVal, double dTime);
	private native int InitInteger(int nInd, int nVal);
	
	public CISLIntData(CISLConnect cConnect, String sId) {
		super(cConnect, sId);
		m_dTimes = new double[2];
		m_dTimes[0] = 0.0;
		m_dTimes[1] = 0.0;
		m_nData = 0;
	}
	
	public CISLIntData(CISLConnect cConnect, String sId, int nCausality) {
		super(cConnect, sId, nCausality, 1, 1);
		m_dTimes = new double[2];
		m_dTimes[0] = 0.0;
		m_dTimes[1] = 0.0;
		m_nData = 0;
	}

	public boolean Initialize(int nVal) {
		if (InitInteger(GetInd(), nVal) < 0) {
			return false;
		}
		return true;
	}

	public int GetData(double dTime, int nWait) {
		m_dTimes[1] = dTime;
		m_nData = GetInt(GetInd(), m_dTimes, nWait);
		StoreInt(GetInd(), m_nData, m_dTimes[0]);
		return m_nData;
	}
	
	public double GetTime() {
		return m_dTimes[0];
	}

	public void SetData(int nVal, double dTime, int nWait) {
		m_dTimes[0] = dTime;
		m_dTimes[1] = dTime;
		m_nData = nVal;
		SetInt(GetInd(), nVal, dTime, nWait);
		StoreInt(GetInd(), nVal, dTime);
	}

	public void SetDataAndStep(int nVal, double dTime, double dStep, int nWait) {
		m_dTimes[0] = dTime;
		m_dTimes[1] = dTime;
		m_nData = nVal;
		SetIntAndStep(GetInd(), nVal, dTime, dStep, nWait);
		StoreInt(GetInd(), nVal, dTime);
	}
}
