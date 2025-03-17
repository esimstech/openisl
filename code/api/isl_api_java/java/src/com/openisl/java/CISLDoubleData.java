/* 
 *     Name: CISLDoubleData.java
 *
 *     Description: Java OpenISL real variable.
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

public class CISLDoubleData extends CISLData {
	private double m_dData;
	private double[] m_dTimes;

	private native double GetDouble(int nInd, double[] dTimes, int nWait);
	private native void SetDouble(int nInd, double dVal, double dTime, int nWait);
	private native void SetDoubleAndStep(int nInd, double dVal, double dTime, double dStep, int nWait);
	private native void StoreDouble(int nInd, double dVal, double dTime);
	private native int InitDouble(int nInd, double dVal);
	
	public CISLDoubleData(CISLConnect cConnect, String sId) {
		super(cConnect, sId);
		m_dTimes = new double[2];
		m_dTimes[0] = 0.0;
		m_dTimes[1] = 0.0;
		m_dData = 0.0;
	}

	public CISLDoubleData(CISLConnect cConnect, String sId, int nCausality) {
		super(cConnect, sId, nCausality, 0, 1);
		m_dTimes = new double[2];
		m_dTimes[0] = 0.0;
		m_dTimes[1] = 0.0;
		m_dData = 0.0;
	}

	public boolean Initialize(double dVal) {
		if (InitDouble(GetInd(), dVal) < 0) {
			return false;
		}
		return true;
	}

	public double GetData(double dTime, int nWait) {
		m_dTimes[1] = dTime;
		m_dData = GetDouble(GetInd(), m_dTimes, nWait);
		StoreDouble(GetInd(), m_dData, m_dTimes[0]);
		return m_dData;
	}
	
	public double GetTime() {
		return m_dTimes[0];
	}

	public void SetData(double dVal, double dTime, int nWait) {
		m_dTimes[0] = dTime;
		m_dTimes[1] = dTime;
		m_dData = dVal;
		SetDouble(GetInd(), dVal, dTime, nWait);
		StoreDouble(GetInd(), dVal, dTime);
	}

	public void SetDataAndStep(double dVal, double dTime, double dStep, int nWait) {
		m_dTimes[0] = dTime;
		m_dTimes[1] = dTime;
		m_dData = dVal;
		SetDoubleAndStep(GetInd(), dVal, dTime, dStep, nWait);
		StoreDouble(GetInd(), dVal, dTime);
	}
}
