
#include <string>
#include <map>
#include "DocumentIndexImpl.h"
#include <memory>
#include <utility>
#include "QueryParser.hpp"
#include "../varint/CompressedSet.h"
#include <set>

using namespace std;

class Engine
{
	public:

		Engine(const string& queryParserDelimiters, char keyWordSplitter) :
			queryParserDelimiters(queryParserDelimiters),
			keyWordSplitter(keyWordSplitter)
		{
			documentIndex = new DocumentIndexImpl();
		}

		~Engine()
		{
			delete documentIndex;
		}

		void addDocument(shared_ptr<IDocument> document)
		{
			documentIndex->addDoc(docId, document);

			auto entries = document->getEntries();

			for (auto iter = entries.begin(); iter != entries.end(); ++iter)
			{
				string key = iter->first;
				string query = iter->second;

				QueryParser qp(query, queryParserDelimiters);

				for (auto token : qp.getTokens())
				{
					string word = key + keyWordSplitter + token;

					auto found = wordIndex.find(word);
					string bitmap;
					CompressedSet set;

					/*
					 * if the word exists in the wordIndex then
					 * we know that a reverse index for it exists
					 */
					if (found != wordIndex.end())
					{
						auto wordId = found->second;

						bitmap = invertedIndex[wordId];
						stringstream bitmapStream(bitmap);
						set.read(bitmapStream);

						// add only if not already marked!
						if (!set.find(docId))
						{
							// no need for unorderedAdd as each document will auto increment
							set.addDoc(docId);
						}

						stringstream ss;
						set.write(ss);
						bitmap = ss.str();

						invertedIndex[wordId] = bitmap;
					}
					else // this is a brand new word
					{
						set.addDoc(docId);

						stringstream ss;
						set.write(ss);
						bitmap = ss.str();

						invertedIndex.insert(make_pair(wordId, bitmap));
						wordIndex.insert(make_pair(word, wordId++));
					}
				}

			} // end looping through entries

			++docId;
		}

		string getDocumentListBitmap(const string& word)
		{
			CompressedSet set;
			string bitmap;

			auto found = wordIndex.find(word);

			if (found != wordIndex.end())
			{
				auto wordId = found->second;
				bitmap = invertedIndex[wordId];
			}

			return move(bitmap);
		}

		set<string> getWords()
		{
			set<string> words;

			for (auto iter = wordIndex.begin(); iter != wordIndex.end(); ++iter)
			{
				words.insert(iter->first);
			}

			return move(words);
		}

	private:

		unsigned long docId = 1;
		unsigned long wordId = 1;

		string queryParserDelimiters;
		char keyWordSplitter;

		// store all the words
		map<string, unsigned int> wordIndex;

		// store all the documents
		IDocumentIndex* documentIndex;

		// inverted index that maps words(wordId) to documents that contain it
		map<unsigned int, string> invertedIndex;

};
