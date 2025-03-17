/*
 *     Name: isl_xml.cpp
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

 /*
  *     Header files
  */

#include <boost/filesystem.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "isl_xml.h"


/*
 *     Classes definition
 */

isl::CXMLNode::CXMLNode()
{

}

isl::CXMLNode::~CXMLNode()
{
	if (m_lNodes.empty() == false) {
		std::vector<CXMLNode *>::iterator iNode = m_lNodes.begin();
		while (iNode != m_lNodes.end()) {
			delete *iNode;
			iNode = m_lNodes.erase(iNode);
		}
	}
}

void isl::CXMLNode::SetNode(const std::string & sNode)
{
	m_sNode = sNode;
}

std::string isl::CXMLNode::GetNodeName()
{
	return m_sNode;
}

void isl::CXMLNode::SetAttribute(const std::string & sName, const std::string & sValue)
{
	m_mAttributes[sName] = sValue;
}

std::vector<std::string> isl::CXMLNode::GetAttributes()
{
	std::vector<std::string> lAtts;
	for (const auto& cAtt : m_mAttributes) {
		lAtts.push_back(cAtt.first);
	}
	return lAtts;
}

std::string isl::CXMLNode::GetAttribute(const std::string & sName)
{
	try {
		return m_mAttributes.at(sName);
	}
	catch (...) {
		return std::string();
	}
}

int isl::CXMLNode::CountNodes()
{
	return (int )(m_lNodes.size());
}

isl::CXMLNode * isl::CXMLNode::AddNode(const std::string & sNode)
{
	CXMLNode * cNode = new CXMLNode();
	cNode->SetNode(sNode);
	m_lNodes.push_back(cNode);
	return cNode;
}

isl::CXMLNode * isl::CXMLNode::GetNode(int i)
{
	if (m_lNodes.empty()) {
		return 0;
	}
	return m_lNodes.at(i);
}

isl::CXMLNode * isl::CXMLNode::GetNode(const std::string & sNode)
{
	for (CXMLNode * cNode : m_lNodes) {
		if (cNode->m_sNode == sNode) {
			return cNode;
		}
	}
	return 0;
}

isl::CXMLNode * isl::CXMLNode::GetNode(const std::string & sNode,
	const std::string & sAttName, const std::string & sAttVal)
{
	for (CXMLNode * cNode : m_lNodes) {
		if (cNode->m_sNode == sNode) {
			if (cNode->GetAttribute(sAttName) == sAttVal) {
				return cNode;
			}
		}
	}
	return 0;
}

//

isl::CXML::CXML(const std::string & sFile)
{
	m_sFile = sFile;
	m_eStatus = SUCCESS;
	m_cRoot = new CXMLNode();
}

isl::CXML::~CXML()
{
	if (m_cRoot != 0) {
		delete m_cRoot;
	}
}

std::string isl::CXML::GetFile()
{
	return m_sFile;
}

isl::CXML::tStatus isl::CXML::GetStatus()
{
	return m_eStatus;
}

std::string isl::CXML::GetParserError()
{
	return m_sParserError;
}

bool isl::CXML::Load()
{
	if (m_sFile.empty()) {
		m_eStatus = NOFILE;
		return false;
	}
	if (boost::filesystem::exists(m_sFile) == false) {
		m_eStatus = FILENOTEXIST;
		return false;
	}
	try {
		boost::property_tree::read_xml(m_sFile, m_cTree);
	}
	catch (const boost::property_tree::xml_parser_error& e) {
		m_sParserError = e.filename() + ":" + std::to_string(e.line()) + ":" + e.message();
		m_eStatus = XMLPARSERERROR;
		return false;
	}
	return true;
}

bool isl::CXML::Save(const std::string & sFile)
{
	if (m_cRoot == 0) {
		return false;
	}
	int nCount = m_cRoot->CountNodes();
	if (nCount == 0) {
		return false;
	}
	m_cTree.clear();
	for (int i = 0; i < nCount; i++) {
		boost::property_tree::ptree bcNextNode;
		BuildTree(m_cTree, bcNextNode, m_cRoot->GetNode(i));
		m_cTree.add_child(m_cRoot->GetNode(i)->GetNodeName(), bcNextNode);
	}
	std::string sFileToSave = m_sFile;
	if (sFile.empty() == false) {
		sFileToSave = sFile;
	}
	try {
		boost::property_tree::write_xml(sFileToSave, m_cTree, std::locale(),
			boost::property_tree::xml_writer_make_settings<std::string>(' ', 4));
	}
	catch (const boost::property_tree::xml_parser_error& e) {
		m_sParserError = e.filename() + ":" + std::to_string(e.line()) + ":" + e.message();
		m_eStatus = XMLPARSERERROR;
		return false;
	}
	return true;
}

bool isl::CXML::Parse()
{
	boost::property_tree::ptree::iterator iElt;
	for (iElt = m_cTree.begin(); iElt != m_cTree.end(); iElt++) {
		ParseElement(m_cRoot, iElt->first, iElt->second);
	}
	return true;
}

void isl::CXML::ParseElement(CXMLNode * cParent, const std::string & sElt,
	boost::property_tree::ptree & cElt)
{
	boost::property_tree::ptree::iterator iElt;
	if (sElt == "<xmlattr>") {
		for (iElt = cElt.begin(); iElt != cElt.end(); iElt++) {
			cParent->SetAttribute(iElt->first, iElt->second.data());
		}
	}
	else {
		CXMLNode * cNode = cParent->AddNode(sElt);
		for (iElt = cElt.begin(); iElt != cElt.end(); iElt++) {
			ParseElement(cNode, iElt->first, iElt->second);
		}
	}
}

void isl::CXML::BuildTree(boost::property_tree::ptree & bcParentNode,
	boost::property_tree::ptree & bcNode, CXMLNode * cNode)
{
	std::vector<std::string> lAtts = cNode->GetAttributes();
	if (lAtts.size() != 0) {
		for (size_t i = 0; i < lAtts.size(); i++) {
			bcNode.put(std::string("<xmlattr>.") + lAtts[i], cNode->GetAttribute(lAtts[i]));
		}
	}
	int nCount = cNode->CountNodes();
	if (nCount != 0) {
		for (int i = 0; i < nCount; i++) {
			boost::property_tree::ptree bcNextNode;
			BuildTree(bcNode, bcNextNode, cNode->GetNode(i));
			bcNode.add_child(cNode->GetNode(i)->GetNodeName(), bcNextNode);
		}
	}
}

isl::CXMLNode * isl::CXML::GetRoot()
{
	return m_cRoot;
}
