/*
 *     Name: isl_xml.h
 *
 *     Description: XML tools.
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

#ifndef _ISL_XML_H_
#define _ISL_XML_H_

/*
 *     Header files
 */

#include <vector>
#include <string>
#include <map>

#include <boost/property_tree/ptree.hpp>


/*
 *     Classes declaration
 */

namespace isl {
	class CXMLNode
	{
	public:
		CXMLNode();
		~CXMLNode();

		void SetNode(const std::string & sNode);
		std::string GetNodeName();

		void SetAttribute(const std::string & sName, const std::string & sValue);
		std::vector<std::string> GetAttributes();
		std::string GetAttribute(const std::string & sName);

		int CountNodes();
		CXMLNode * AddNode(const std::string & sNode);
		CXMLNode * GetNode(int i);
		CXMLNode * GetNode(const std::string & sNode);
		CXMLNode * GetNode(const std::string & sNode,
			const std::string & sAttName, const std::string & sAttVal);

	private:
		std::string m_sNode;
		std::map<std::string, std::string> m_mAttributes;

		std::vector<CXMLNode *> m_lNodes;
	};

	class CXML
	{
	public:
		enum tStatus {
			SUCCESS = 0,
			NOFILE,
			FILENOTEXIST,
			XMLPARSERERROR,
			UNKNOWN
		};

		CXML(const std::string & sFile);
		~CXML();

		std::string GetFile();
		tStatus GetStatus();
		std::string GetParserError();

		bool Load();
		bool Save(const std::string & sFile = "");

		bool Parse();

		CXMLNode * GetRoot();

	private:
		void ParseElement(CXMLNode * cParent, const std::string & sElt,
			boost::property_tree::ptree & cElt);
		void BuildTree(boost::property_tree::ptree & bcParentNode,
			boost::property_tree::ptree & bcNode, CXMLNode * cNode);

	private:
		std::string m_sFile;
		boost::property_tree::ptree m_cTree;

		tStatus m_eStatus;
		std::string m_sParserError;

		CXMLNode * m_cRoot;
	};
}

#endif // _IPI_XML_H_
