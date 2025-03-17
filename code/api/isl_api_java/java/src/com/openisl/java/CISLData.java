/* 
 *     Name: CISLData.java
 *
 *     Description: Java OpenISL variable.
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
