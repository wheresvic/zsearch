
#include <string>
#include <vector>
#include <memory>
#include <utility>
#include "ITokenizer.h"

using namespace std;

class QueryParser
{
	public:

		QueryParser(const string& input, shared_ptr<ITokenizer> tokenizer) : query(input), tokenizer(tokenizer)
		{ }

		vector<string> getTokens()
		{
			tokenizer->setString(query);

			vector<string> tokens;

			while (tokenizer->nextToken())
			{
				tokens.push_back(tokenizer->getToken());
			}

			return tokens;
		}

	private:

		const string query;
		shared_ptr<ITokenizer> tokenizer;

};

