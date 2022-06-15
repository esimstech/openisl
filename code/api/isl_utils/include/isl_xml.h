/*
 *     Name: isl_xml.h
 *
 *     Description: XML tools.
 *
 *     Modification date: 26/02/2022
 *     Author: T. Roudier
 *     Copyright (c) E-Sim Solutions Inc 2020-2022
 *
 *     Distributed under the OpenISL Software License, Version 1.0.
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
