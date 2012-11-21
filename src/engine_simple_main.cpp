
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <sstream>

// #include "../varint/CompressedSet.h"

#include "DocumentImpl.h"
#include "TokenizerImpl.h"
#include "DocumentStoreSimple.h"
#include "KVStoreLevelDb.h"
#include "KVStoreInMemory.h"
#include "Engine.hpp"
#include "Constants.hpp"
#include "Word.hpp"
#include "varint/CompressedSet.h"
#include "varint/BasicSet.h"
#include "varint/SetFactory.h"

using namespace std;

string convertInt(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}

int main()
{
	string input;

	char documentDelimiter = ' ';
	int documentId = 500;

	shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>(zsearch::QUERY_PARSER_DELIMITERS);
	shared_ptr<IDocumentStore> documentStore = make_shared<DocumentStoreSimple>();
	shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreLevelDb>("/tmp/InvertedIndex");
	SetType setType = BasicSet_t;
	
	Engine engine(tokenizer, documentStore, invertedIndexStore, setType);

	cout << "Made engine!" << endl;
	
	// test input
	while (getline(cin, input))
	{
		// cout << input;
		string title = convertInt(documentId++);
		shared_ptr<IDocument> doc = make_shared<DocumentImpl>(); // (new DocumentImpl());
		// doc->setTitle(title);
		doc->addEntry("title", title);

		// parse the input, each line is a single document
		size_t found = input.find_first_of(documentDelimiter);
		if (found != string::npos)
		{
			string field = input.substr(0, found);
			string value = input.substr(found + 1);
			// cout << "field : " << field << ", value: " << value << endl;
			doc->addEntry(field, value);
		}
		else
		{
			throw "Couldn't split key value!";
		}

		cout << "Added document: " << engine.addDocument(doc) << endl;
	}

	// test that searching for some more text returns only 1 document

	string query = "some  more text";
	cout << "searching for: " << query << endl;

	auto docSet = engine.search(query);

	for (auto document : docSet)
	{
		string title;
		document->getEntry("title", title);
		cout << title << " ";
	}

	cout << endl;

	query = "bandes";
	cout << "searching for: " << query << endl;

	docSet = engine.search(query);

	for (auto document : docSet)
	{
		string title;
		document->getEntry("title", title);
		cout << title << " ";
	}

	cout << endl;

	return 0;
}

