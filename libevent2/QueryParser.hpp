
#include <string>
#include <vector>
#include "Tokenizer.hpp"

using namespace std;

class QueryParser
{
	public:

		QueryParser(const string& input, const string& delim) : query(input), delimiters(delim)
		{ }

		/*
		void setQuery(const string& input)
		{
			query = input;
		}
		*/
		
		const vector<string>& getTokens()
		{
			Tokenizer s(query, delimiters);

			while (s.nextToken())
			{
				tokens.push_back(s.getToken());
			}

			return tokens;
		}
		
		/*
		void clear()
		{
			query.clear();
			tokens.clear();
		}
		*/
		
	private:

		const string query;
		const string delimiters;
		vector<string> tokens;

};

