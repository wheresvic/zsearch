
#include <string>
#include <map>
#include "DocumentStoreSimple.h"
#include <memory>
// #include <utility>
#include "QueryParser.hpp"
#include <set>
#include <vector>
#include "Word.hpp"

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
				string field = iter->first;
				string value = iter->second;

				fields.insert(field);

				QueryParser qp(value, tokenizer);

				vector<string> tokens = qp.getTokens();

				for (auto token : tokens)
				{
					// string word = field + keyWordSplitter + token;
					Word word(field, token);

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

		set<Word> getWords()
		{
			set<Word> words;

			for (auto iter = wordIndex.begin(); iter != wordIndex.end(); ++iter)
			{
				words.insert(iter->first);
			}

			// http://stackoverflow.com/questions/12666362/c-stdmove-confusion
			return words;
		}

		bool findInSets(const vector<set<unsigned int>>& documentSets, unsigned int docId)
		{
			for (auto documentSet : documentSets)
			{
				if (documentSet.find(docId) == documentSet.end())
				{
					return false;
				}
			}

			return true;
		}


		/**
		 * Return documents that contain all the words in the query
		 */
		set<shared_ptr<IDocument>> search(const string& query)
		{
			// q = "", field1="some more text" = input1/some input1/more input1/text
			// q = "some more text" = (input1/some OR input2/some ) AND (input1/more or input2/more)

			set<shared_ptr<IDocument>> documentSet;

			set<string> queryTokens;

			QueryParser qp(query, tokenizer);
			vector<string> tokens = qp.getTokens();

			for (auto token : tokens)
			{
				queryTokens.insert(token);
			}

			auto documents = documentStore->getDocuments();

			// some 500 501
			// more 501
			// test 500 501

			vector<set<unsigned int>> tokenDocumentSets;

			for (auto token : queryTokens)
			{
				set<unsigned int> unionDocumentSet;

				for (auto field : fields)
				{
					Word word(field, token);
					auto found = wordIndex.find(word);

					if (found != wordIndex.end())
					{
						auto wordId = found->second;
						auto docSet = invertedIndex[wordId];

						for (auto id : docSet)
						{
							unionDocumentSet.insert(id);
						}
					}
				}

				tokenDocumentSets.push_back(unionDocumentSet);
			}

			for (auto tokenDocumentSet : tokenDocumentSets)
			{
				for (auto docId : tokenDocumentSet)
				{
					if (findInSets(tokenDocumentSets, docId))
					{
						documentSet.insert(documents[docId]);
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
		map<Word, unsigned int> wordIndex;

		// store all the fields
		set<string> fields;

		// store all the documents
		shared_ptr<IDocumentStore> documentStore;

		// inverted index that maps words(wordId) to documents that contain it
		map<unsigned int, set<unsigned int>> invertedIndex;

};
