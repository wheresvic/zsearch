#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>

#include "lib/tpunit++.hpp"
#include "src/DocumentImpl.h"
#include "TestUtils.hpp"

using namespace std;



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
		string docStr;
		try
		{
			docStr = readFile("document01.xml");
		}
		catch (const string& e)
		{
			TRACE(e.c_str());
			ABORT();	
		}
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
		string docStr;
		try
		{
			docStr = readFile("document02.xml");
		}
		catch (const string& e)
		{
			TRACE(e.c_str());
			ABORT();
		}
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