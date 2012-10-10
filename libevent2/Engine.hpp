
#include <string>
#include <map>
#include "DocumentStoreImpl.h"
#include <memory>
// #include <utility>
#include "QueryParser.hpp"
#include <set>

using namespace std;

class Engine
{
	public:

		//
		// http://stackoverflow.com/questions/8385457/should-i-pass-a-shared-ptr-by-reference
		//
		Engine(const char keyWordSplitter,
				shared_ptr<ITokenizer> tokenizer,
				shared_ptr<IDocumentStore> documentStore) :
			keyWordSplitter(keyWordSplitter),
			tokenizer(tokenizer),
			documentStore(documentStore)
		{

		}

		unsigned int addDocument(shared_ptr<IDocument> document)
		{
			documentStore->addDoc(docId, document);

			auto entries = document->getEntries();

			for (auto iter = entries.begin(); iter != entries.end(); ++iter)
			{
				string key = iter->first;
				string query = iter->second;

				QueryParser qp(query, tokenizer);

				vector<string> tokens = qp.getTokens();

				for (auto token : tokens)
				{
					string word = key + keyWordSplitter + token;

					auto found = wordIndex.find(word);

					/*
					 * if the word exists in the wordIndex then
					 * we know that a reverse index for it exists
					 */
					if (found != wordIndex.end())
					{
						auto wordId = found->second;
						invertedIndex[wordId].insert(docId);
					}
					else // this is a brand new word
					{
						set<unsigned int> docSet;
						docSet.insert(docId);

						invertedIndex.insert(make_pair(wordId, docSet));
						wordIndex.insert(make_pair(word, wordId++));
					}
				}

			} // end looping through entries

			return docId++;
		}

		/*
		set<shared_ptr<IDocument>> getDocumentSet(const string& word)
		{
			set<shared_ptr<IDocument>> documentSet;

			auto found = wordIndex.find(word);

			if (found != wordIndex.end())
			{
				auto documents = documentIndex->getDocuments();

				auto wordId = found->second;
				auto docSet = invertedIndex[wordId];

				for (auto id : docSet)
				{
					documentSet.insert(documents[id]);
				}
			}

			return move(documentSet);
		}
		*/

		/*
		set<unsigned int> getDocumentIdSet(const set<string>& words)
		{
			set<unsigned int> documentSet;

			for (auto word : words)
			{
				auto found = wordIndex.find(word);

				if (found != wordIndex.end())
				{
					auto wordId = found->second;
					auto docSet = invertedIndex[wordId];

					for (auto id : docSet)
					{
						documentSet.insert(id);
					}
				}
			}

			return move(documentSet);
		}
		*/

		set<string> getWords()
		{
			set<string> words;

			for (auto iter = wordIndex.begin(); iter != wordIndex.end(); ++iter)
			{
				words.insert(iter->first);
			}

			// http://stackoverflow.com/questions/12666362/c-stdmove-confusion
			return words;
		}


		set<shared_ptr<IDocument>> search(const set<string>& queryTokens)
		{
			set<shared_ptr<IDocument>> documentSet;

			auto documents = documentStore->getDocuments();

			for (auto token : queryTokens)
			{
				auto found = wordIndex.find(token);

				if (found != wordIndex.end())
				{
					auto wordId = found->second;
					auto docSet = invertedIndex[wordId];

					for (auto id : docSet)
					{
						documentSet.insert(documents[id]);
					}
				}
			}

			return documentSet;
		}

	private:

		unsigned long docId = 0;
		unsigned long wordId = 0;

		string queryParserDelimiters;
		char keyWordSplitter;

		// tokenizer
		shared_ptr<ITokenizer> tokenizer;

		// store all the words
		map<string, unsigned int> wordIndex;

		// store all the documents
		shared_ptr<IDocumentStore> documentStore;

		// inverted index that maps words(wordId) to documents that contain it
		map<unsigned int, set<unsigned int>> invertedIndex;

};
