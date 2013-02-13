
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>

#include "src/ITokenizer.h"
#include "src/TokenizerImpl.h"
#include "src/Constants.hpp"
#include "lib/tpunit++.hpp"
#include "TestUtils.hpp"

using namespace std;


/**
 * Test the tokenizer
 */
struct TokenizerTest : tpunit::TestFixture
{
	TokenizerTest() : tpunit::TestFixture
	(
		TEST(TokenizerTest::testTokenizing)
	)
	{ }

	void testTokenizing()
	{
		string text(" snoop  doggy dawg");

		shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>();

		tokenizer->setString(text);
		
		while (tokenizer->nextToken())
		{
			string token = tokenizer->getToken();
			cout << token << endl;			
		}
		
		/*
		vector<string> words = qp.getTokens();

		ASSERT_EQUAL(words.size(), 3);
		ASSERT_EQUAL(words[0].compare("snoop"), 0);
		ASSERT_EQUAL(words[1].compare("doggy"), 0);
		ASSERT_EQUAL(words[2].compare("dawg"), 0);
		*/
	}

};