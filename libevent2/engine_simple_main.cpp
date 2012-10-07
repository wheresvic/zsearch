

#include "DocumentImpl.h"
// #include "../varint/CompressedSet.h"
#include "EngineSet.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <set>

using namespace std;

int main()
{
	string input;
	string queryParserDelimiters = " \t\n\r.,";
	char keyWordSplitter = '/';

	char documentDelimiter = ' ';

	EngineSet engine(queryParserDelimiters, keyWordSplitter);

	// test input
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

		engine.addDocument(doc);
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

		cout << endl;
	}


	return 0;
}
