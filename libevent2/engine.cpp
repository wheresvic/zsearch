
#include "QueryParser.hpp"
#include "DocumentIndexerImpl.h"
#include "DocumentImpl.h"
#include "../varint/CompressedSet.h"
#include <iostream>
#include <string>
#include <vector>
#include <memory>

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
	char keyWordSplitter = '/';

	unsigned long docId = 0;

	map<string, unsigned long> words;
	map<unsigned long, CompressedSet> invertedIndex;

	shared_ptr<IDocumentIndexer> documentStore = make_shared<DocumentIndexerImpl>();

	while (getline(cin, input))
	{
		// cout << input;
		shared_ptr<IDocument> doc = make_shared<DocumentImpl>(); // (new DocumentImpl());

		// parse the input, each line is a single document
		size_t found = input.find_first_of(documentDelimiter);
		if (found != string::npos)
		{
			doc->addEntry(input.substr(0, found), input.substr(found + 1));
		}
		else
		{
			throw "Couldn't split key value!";
		}

		documentStore->addDoc(docId++, doc);
	}

	auto documents = documentStore->getDocuments();

	for (auto iter = documents.begin(); iter != documents.end(); ++iter)
	{
		cout << "document id: " << iter->first << endl;

		shared_ptr<IDocument> doc = iter->second;

		// we know that there's only 1 entry for a document
		string key = doc->getEntries().begin()->first;
		string query = doc->getEntries().begin()->second;

		cout << key << " " << query << endl;

		QueryParser qp(query, queryParserDelimiters);

		for (auto token : qp.getTokens())
		{
			string word = key + keyWordSplitter + token;
			cout << word << endl;
		}

	}

	return 0;
}
