
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include "QueryParser.hpp"
#include "rapidxml.hpp"
#include "tpunit++.hpp"
#include "Constants.hpp"


using namespace std;

/**
 * Test the query parser
 */
struct QueryParserTest : tpunit::TestFixture
{
	QueryParserTest() : tpunit::TestFixture
	(
		TEST(QueryParserTest::testTokenizing)
	)
	{ }

	void testTokenizing()
	{
		string text(" snoop  doggy dawg");

		QueryParser qp(text, QUERY_PARSER_DELIMITERS);

		vector<string> words;

		for (auto token : qp.getTokens())
		{
			words.push_back(token);
		}

		ASSERT_EQUAL(words.size(), 3);
		ASSERT_EQUAL(words[0].compare("snoop"), 0);
		ASSERT_EQUAL(words[1].compare("doggy"), 0);
		ASSERT_EQUAL(words[2].compare("dawg"), 0);
	}

} __QueryParserTest; // this name is really important!

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
		ifstream fin("document01.xml");

		if (fin.fail())
			ABORT();

		fin.seekg(0, ios::end);
		size_t length = fin.tellg();
		fin.seekg(0, ios::beg);
		char* buffer = new char[length + 1];
		fin.read(buffer, length);
		buffer[length] = '\0';

		fin.close();

		rapidxml::xml_document<> doc;				// character type defaults to char
		doc.parse<rapidxml::parse_full>(buffer);	// 0 means default parse flags

		delete [] buffer;

		string root(doc.first_node()->name());
		ASSERT_EQUAL(root.compare("document"), 0);

		vector<string> fields;

		for (rapidxml::xml_node<>* n = doc.first_node()->first_node(); n; n = n->next_sibling())
		{
			string name(n->name());
			string value(n->value());
			fields.push_back(name + ":" + value);
		}

		ASSERT_EQUAL(fields.size(), 3);
		ASSERT_EQUAL(fields[0].compare("title:Input document"), 0);
		ASSERT_EQUAL(fields[1].compare("input1: some text"), 0);
		ASSERT_EQUAL(fields[2].compare("input1: some more text"), 0);
	}

} __XmlTest;


int main()
{
   /**
    * Run all of the registered tpunit++ tests. Returns 0 if
    * all tests are successful, otherwise returns the number
    * of failing assertions.
    */
   return tpunit::Tests::Run();

}
