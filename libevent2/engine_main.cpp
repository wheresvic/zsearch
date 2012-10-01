
#include "QueryParser.hpp"
#include "DocumentIndexImpl.h"
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

	unsigned int docId = 0;
	unsigned int wordId = 0;

	// store all the words
	map<string, unsigned int> wordIndex;
	// store all the documents
	shared_ptr<IDocumentIndex> documentIndex = make_shared<DocumentIndexImpl>();
	// inverted index that maps words(wordId) to documents that contain it
	map<unsigned int, string> invertedIndex;

	// helper, word lookup
	map<unsigned int, string> wordLookup;

	// test input, build the documentIndex
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

		documentIndex->addDoc(docId++, doc);
	}

	/**
	 * we now loop through the documents
	 * split the words up and index them
	 * along with adding them to the inverted index
	 */
	auto documents = documentIndex->getDocuments();

	for (auto iter = documents.begin(); iter != documents.end(); ++iter)
	{

		unsigned int docId = iter->first;
		shared_ptr<IDocument> doc = iter->second;

		cout << "document id: " << docId << endl;

		// we know that there's only 1 entry for a document
		string key = doc->getEntries().begin()->first;
		string query = doc->getEntries().begin()->second;

		cout << key << " " << query << endl;

		QueryParser qp(query, queryParserDelimiters);

		for (auto token : qp.getTokens())
		{
			string word = key + keyWordSplitter + token;

			auto found = wordIndex.find(word);
			string bitmap;
			CompressedSet set;

			if (found != wordIndex.end())
			{
				// cout << "found!" << endl;

				auto wordId = found->second;

				bitmap = invertedIndex[wordId];
				stringstream bitmapStream(bitmap);
				set.read(bitmapStream);

				// no need for unorderedAdd as we're going through the
				// documents sequentially
				set.addDoc(docId);

				stringstream ss;
				set.write(ss);
				bitmap = ss.str();

				invertedIndex[wordId] = bitmap;
			}
			else
			{
				// cout << "not found" << endl;

				wordIndex.insert(make_pair(word, wordId++));
				wordLookup.insert(make_pair(wordId - 1, word));

				set.addDoc(docId);

				stringstream ss;
				set.write(ss);
				bitmap = ss.str();

				invertedIndex.insert(make_pair(wordId - 1, bitmap));
			}
		}

	}

	cout << "Done processing input " << endl;

	for (auto iter = invertedIndex.begin(); iter != invertedIndex.end(); ++iter)
	{
		auto wordId = iter->first;
		auto bitmap = iter->second;

		CompressedSet set;
		stringstream bitmapStream(bitmap);
		set.read(bitmapStream);

		cout << wordLookup[wordId] << " : ";

		CompressedSet::Iterator it(&set);

		for (; it.docID() != NO_MORE_DOCS; it.nextDoc())
		{
			cout << it.docID() << " ";
		}

		cout << endl;
	}

	return 0;
}
