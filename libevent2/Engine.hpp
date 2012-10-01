
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

						set.addDoc(docId);

						stringstream ss;
						set.write(ss);
						bitmap = ss.str();

						invertedIndex.insert(make_pair(wordId, bitmap));
						wordIndex.insert(make_pair(word, wordId++));

					}
				}

			} // end looping through entries

			documentIndex->addDoc(docId++, document);
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

		unsigned long docId = 0;
		unsigned long wordId = 0;

		string queryParserDelimiters;
		char keyWordSplitter;

		// store all the words
		map<string, unsigned int> wordIndex;

		// store all the documents
		IDocumentIndex* documentIndex;

		// inverted index that maps words(wordId) to documents that contain it
		map<unsigned int, string> invertedIndex;

};
