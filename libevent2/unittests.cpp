
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>
#include <utility>

#include "DocumentImpl.h"
#include "TokenizerImpl.h"
#include "QueryParser.hpp"
#include "rapidxml.hpp"
#include "tpunit++.hpp"
#include "Constants.hpp"


using namespace std;

string readFile(const string& fileName)
{
	ifstream fin(fileName.c_str());

	if (fin.fail())
		throw "Could not open " + fileName + "!";

	fin.seekg(0, ios::end);
	size_t length = fin.tellg();
	fin.seekg(0, ios::beg);
	char* buffer = new char[length + 1];
	fin.read(buffer, length);
	buffer[length] = '\0';

	fin.close();

	string fileStr(buffer);
	delete [] buffer;

	return move(fileStr);
}

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

		shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>(zsearch::QUERY_PARSER_DELIMITERS);

		QueryParser qp(text, tokenizer);

		vector<string> words = qp.getTokens();

		ASSERT_EQUAL(words.size(), 3);
		ASSERT_EQUAL(words[0].compare("snoop"), 0);
		ASSERT_EQUAL(words[1].compare("doggy"), 0);
		ASSERT_EQUAL(words[2].compare("dawg"), 0);
	}

};

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
		string fileStr = readFile("document01.xml");

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

/**
 * Test DocumentImpl
 */
struct DocumentImplTest : tpunit::TestFixture
{
	DocumentImplTest() : tpunit::TestFixture
	(
		TEST(DocumentImplTest::testParsingDocumentOk),
		TEST(DocumentImplTest::testParsingDocumentNoTitle),
		TEST(DocumentImplTest::testParsingDocumentBad),
		TEST(DocumentImplTest::testParsingDocumentEmptyField),
		TEST(DocumentImplTest::testParsingEmptyString),
		TEST(DocumentImplTest::testParsingDocumentCDATA)
	)
	{ }

	void testParsingDocumentOk()
	{
		string docStr = readFile("document01.xml");
		EXPECT_NO_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr));
	}

	void testParsingDocumentNoTitle()
	{
		string docStr = "<document><input1> some text</input1><input1> some more text</input1></document>";
		EXPECT_NO_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr));
	}

	void testParsingDocumentBad()
	{
		string docStr = "<document1><input1> some text</input1><input1> some more text</input1></document1>";
		EXPECT_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr), string);
	}

	void testParsingDocumentEmptyField()
	{
		string docStr = "<document><input1> some text</input1><input1></input1></document>";
		EXPECT_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr), string);
	}

	void testParsingEmptyString()
	{
		string docStr = "";
		EXPECT_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr), string);
	}

	void testParsingDocumentCDATA()
	{
		string docStr = readFile("document02.xml");
		EXPECT_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr), string);

		/*
		try
		{
			shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr);
		}
		catch (const string& e)
		{
			cout << e << endl;
		}
		*/
	}

};


int main()
{
	QueryParserTest __QueryParserTest;
	XmlTest __XmlTest;
	DocumentImplTest __DocumentImplTest;

	/**
	 * Run all of the registered tpunit++ tests. Returns 0 if
	 * all tests are successful, otherwise returns the number
	 * of failing assertions.
	 */
	return tpunit::Tests::Run();

}
