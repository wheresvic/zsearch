
#include <iostream>
#include <string>
#include <vector>
#include "rapidxml.hpp"
#include "rapidxml_print.hpp"

using namespace std;
using namespace rapidxml;

int main()
{
	vector<char> xmlText;
	string input;

	while (getline(cin, input))
	{
		xmlText.insert(xmlText.end(), input.begin(), input.end());
	}

	xmlText.push_back('\0');

	xml_document<> doc;    					// character type defaults to char
	doc.parse<parse_full>(&xmlText[0]);    	// 0 means default parse flags

	// xml_node<>* root = doc.first_node("document");

	cout << "The first node is '" << doc.first_node()->name() << "'\n";

	for (xml_node<>* n = doc.first_node("document")->first_node(); n; n = n->next_sibling())
	{
		char* v = n->value();
		// if (!v || !*v) v = "(empty)";
		cout << n->name() << " : " << v << '\n';
	}

	string document;
	rapidxml::print(std::back_inserter(document), doc,0);
	cout << document;

	return 0;
}
