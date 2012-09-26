
#include <string>
#include <vector>
#include "Tokenizer.hpp"

using namespace std;

class QueryParser
{
	public:

		QueryParser(const string& input, const string& delim) : query(input), delimiters(delim)
		{ }

		const vector<string>& getTokens()
		{
			Tokenizer s(query, delimiters);

			while (s.NextToken())
			{
				tokens.push_back(s.GetToken());
			}

			return tokens;
		}

	private:

		const string query;
		const string delimiters;
		vector<string> tokens;

};

