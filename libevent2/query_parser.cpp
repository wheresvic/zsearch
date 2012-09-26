
#include "QueryParser.hpp"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

int main()
{
	string input;

	while (getline(cin, input))
	{
		cout << input;

		QueryParser qp(input, " \t\n\r.,");

		const vector<string>& tokens = qp.getTokens();
		cout << tokens.size() << endl;

		for (auto token : tokens)
		{
			cout << token << endl;
		}
	}

	return 0;
}
