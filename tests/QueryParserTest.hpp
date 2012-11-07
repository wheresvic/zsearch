
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>

#include "../src/QueryParser.hpp"
#include "../src/TokenizerImpl.h"
#include "../src/Constants.hpp"
#include "../src/lib/tpunit++.hpp"
#include "TestUtils.hpp"

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

		shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>(zsearch::QUERY_PARSER_DELIMITERS);

		QueryParser qp(text, tokenizer);

		vector<string> words = qp.getTokens();

		ASSERT_EQUAL(words.size(), 3);
		ASSERT_EQUAL(words[0].compare("snoop"), 0);
		ASSERT_EQUAL(words[1].compare("doggy"), 0);
		ASSERT_EQUAL(words[2].compare("dawg"), 0);
	}

};