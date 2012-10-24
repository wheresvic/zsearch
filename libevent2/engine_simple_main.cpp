
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>
#include <sstream>

// #include "../varint/CompressedSet.h"

#include "DocumentImpl.h"
#include "TokenizerImpl.h"
#include "DocumentStoreImpl.h"

#include "Engine.hpp"
#include "Constants.hpp"

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
	shared_ptr<IDocumentStore> documentStore = make_shared<DocumentStoreImpl>();

	Engine engine(zsearch::KEYWORD_SPLITTER, tokenizer, documentStore);

	// test input
	while (getline(cin, input))
	{
		// cout << input;
		string title = convertInt(documentId++);
		shared_ptr<IDocument> doc = make_shared<DocumentImpl>(); // (new DocumentImpl());
		doc->setTitle(title);

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

		cout << "Added document: " << engine.addDocument(doc) << endl;
	}

	set<string> words = engine.getWords();

	for (string word : words)
	{
		cout << word << " ";

		/*
		string bitmap = engine.getDocumentListBitmap(word);

		CompressedSet set;
		stringstream bitmapStream(bitmap);
		set.read(bitmapStream);

		CompressedSet::Iterator it(&set);

		for (; it.docID() != NO_MORE_DOCS; it.nextDoc())
		{
			cout << it.docID() << " ";
		}
		*/

		/*
		auto docIdSet = engine.getDocumentIdSet(wordSet);

		for (auto id : docIdSet)
		{
			cout << id << " ";
		}

		cout << endl;
		*/

		auto docSet = engine.search(word);

		for (auto document : docSet)
		{
			cout << document->getTitle() << " ";
		}

		cout << endl;
	}

	// test that searching for some more text returns only 1 document

	string query = "some  more text";

	auto docSet = engine.search(query);

	for (auto document : docSet)
	{
		cout << document->getTitle() << " ";
	}

	cout << endl;

	return 0;
}



