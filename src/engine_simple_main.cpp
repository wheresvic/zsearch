
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
#include "KVStoreInMemory.hpp"
#include "Engine.hpp"
#include "Constants.hpp"
#include "Word.hpp"
#include "varint/CompressedSet.h"
#include "varint/BasicSet.h"
#include "varint/SetFactory.h"
#include "varint/BasicSetFactory.h"

using namespace std;

string convertInt(int number)
{
   stringstream ss;
   ss << number;
   return ss.str();
}

void search(const string& query, const Engine& engine, unsigned int start, unsigned int offset)
{
	cout << "searching for: " << query << " with start = " << start << ", offset = " << offset << endl;

	auto docIdSet = engine.search(query, start, offset);

	/*
	auto docSet = engine.getDocs(docIdSet);

	for (auto document : docSet)
	{
		string title;
		document->getEntry("title", title);
		cout << title << " ";
	}
	*/

	for (auto docId : docIdSet)
	{
		cout << docId << " ";
	}

	cout << endl;
}

int main()
{
	string input;

	char documentDelimiter = ' ';
	int documentId = 500;

	shared_ptr<ISetFactory> setFactory = make_shared<SetFactory>();
	// shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();

	shared_ptr<ITokenizer> tokenizer = make_shared<TokenizerImpl>(zsearch::QUERY_PARSER_DELIMITERS);
	shared_ptr<IDocumentStore> documentStore = make_shared<DocumentStoreSimple>();
	shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreLevelDb>("/tmp/InvertedIndex");

	Engine engine(tokenizer, documentStore, invertedIndexStore, setFactory);

	engine.disableBatching();

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

	// flush
	engine.flushBatch();

	// test that searching for some more text returns only 1 document

	string query = "some  more text";
	search(query, engine, 0, 0);

	// engine.deleteDocument(2);	// deletes some more text
	search(query, engine, 0, 0);

	query = "série";
	search(query, engine, 0, 0);

	query = "de";
	search(query, engine, 0, 0);
	search(query, engine, 0, 1);
	search(query, engine, 1, 1);
	search(query, engine, 5, 5);
	search(query, engine, 4, 7);
	search(query, engine, 2, 0);




	return 0;
}

