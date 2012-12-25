#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>
#include <sstream>

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
		TEST(DocumentImplTest::testParsingDocumentCDATA),
		TEST(DocumentImplTest::testDocumentWrite),
		TEST(DocumentImplTest::testDocumentXmlEntity),
		TEST(DocumentImplTest::testParsingDocumentTerrible)
	)
	{ }

	void testParsingDocumentOk()
	{
		string docStr;
		try
		{
			docStr = readFile("../data/document01.xml");
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
			docStr = readFile("../data/document02.xml");
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

	void testDocumentWrite()
	{
		string docStr = "<document><input1> some text</input1><input2> some more text</input2></document>";

		shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr);

		stringstream ss;
		document->write(ss);

		string docStrExpected = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" + docStr;
		string docStrActual = ss.str();

		// cout << docStrExpected << endl << docStrActual << endl;

		ASSERT_TRUE(docStrExpected.compare(docStrActual) == 0);
	}

	void testDocumentXmlEntity()
	{
		string docStr;
		try
		{
			docStr = readFile("../data/document04.xml");
		}
		catch (const string& e)
		{
			TRACE(e.c_str());
			ABORT();
		}
		EXPECT_NO_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr));

		shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr);

		stringstream ss;
		document->write(ss);

		string docStrActual = ss.str();

		cout << docStr << endl << docStrActual << endl;

		// ASSERT_TRUE(docStr.compare(docStrActual) == 0);
	}

	void testParsingDocumentTerrible()
	{
		string docStr = "%3Cdocument%3E%0D%0A%09%3Ctitle%3EInput+document%3C%2Ftitle%3E%0D%0A%09%3Cinput1%3E+some+text%3C%2Finput1%3E%0D%0A%09%3Cinput1%3E+some+more+text%3C%2Finput1%3E%0D%0A%3C%2Fdocument%3E";
		EXPECT_THROW(shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr), string);

		// shared_ptr<IDocument> document = make_shared<DocumentImpl>(docStr);


		// auto& entries = document->getEntries();

		/*
		for (auto it = entries.begin(); it != entries.end(); ++ it)
		{
			cout << it->first << " " << it->second << endl;
		}
		*/

		/*
		stringstream ss;
		document->write(ss);

		string docStrActual = ss.str();
		cout << docStrActual << endl;
		*/

	}

};
