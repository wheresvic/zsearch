
#include "QueryParser.hpp"
#include "Document.hpp"
#include "../varint/CompressedSet.h"
#include <iostream>
#include <string>
#include <vector>

using namespace std;

/*
Map<wordid,CompressedSet> invertedIndex:
Map<docid,Document> documentStore;
*/
int main()
{
	string input;
	string queryParserDelimiters = " \t\n\r.,";
	char documentDelimiter = ' ';

	unsigned long docId = 0;
	
	map<unsigned long, CompressedSet> invertedIndex;
	map<unsigned long, Document> documentStore;
	
	while (getline(cin, input))
	{
		// cout << input;
		Document doc;
	
		size_t found = input.find_first_of(documentDelimiter);
		
		if (found != string::npos)
		{
			doc.addEntry(input.substr(0, found), input.substr(found + 1));		
		}
		else
		{
			throw "Couldn't split key value!";
		}
	
		documentStore.insert(make_pair(docId++, doc));
		
	}
	
	for (auto iter = documentStore.begin(); iter != documentStore.end(); ++iter)
	{
		Document doc = iter->second;
		string key = doc.getEntries().begin()->first;
		string query = doc.getEntries().begin()->second;
		
		cout << key << " " << query << endl;
	}
	
	return 0;
}
