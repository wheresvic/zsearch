
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>


#include "../src/lib/rapidxml.hpp"
#include "../src/lib/tpunit++.hpp"
#include "TestUtils.hpp"

using namespace std;

/**
 * Test rapid xml
 */
struct XmlTest : tpunit::TestFixture
{
	XmlTest() : tpunit::TestFixture
	(
		TEST(XmlTest::testParsingDocument)
	)
	{ }

	void testParsingDocument()
	{
		string fileStr;

		try
		{
			fileStr = readFile("document01.xml");
		}
		catch(const string& e)
		{
			TRACE(e.c_str());
			ABORT();
		}

		vector<char> xmlVec;
		copy(fileStr.begin(), fileStr.end(), back_inserter(xmlVec));
		xmlVec.push_back('\n');

		rapidxml::xml_document<> doc;					// character type defaults to char
		doc.parse<rapidxml::parse_full>(&xmlVec[0]);	// 0 means default parse flags

		string root(doc.first_node()->name());
		ASSERT_EQUAL(root.compare("document"), 0);

		vector<string> fields;

		for (rapidxml::xml_node<>* n = doc.first_node()->first_node(); n; n = n->next_sibling())
		{
			string name(n->name());
			// char* v = n->value();
			// if (!v || !*v) v = "(empty)";
			string value(n->value());
			fields.push_back(name + ":" + value);
		}

		ASSERT_EQUAL(fields.size(), 3);
		ASSERT_EQUAL(fields[0].compare("title:Input document"), 0);
		ASSERT_EQUAL(fields[1].compare("input1: some text"), 0);
		ASSERT_EQUAL(fields[2].compare("input1: some more text"), 0);
	}

};

