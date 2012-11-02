
#include <string>
#include <map>
#include "DocumentStoreSimple.h"
#include <memory>
// #include <utility>
#include "QueryParser.hpp"
#include <set>
#include <vector>
#include "Word.hpp"
#include "InvertedIndexSimple.hpp"
#include "../varint/CompressedSet.h"
#include "../varint/LazyOrSet.h"
#include "../varint/LazyAndSet.h"
using namespace std;

class Engine
{
	public:

		//
		// http://stackoverflow.com/questions/8385457/should-i-pass-a-shared-ptr-by-reference
		//
		Engine(	shared_ptr<ITokenizer> tokenizer,
				shared_ptr<IDocumentStore> documentStore) :
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
					Word word(field, token);

					auto found = wordIndex.find(word);
                    if (found != wordIndex.end()){
	                   unsigned int id = found->second;
					   invertedIndex.add(id,docId);
                    } else {
 					   wordIndex.insert(make_pair(word,wordId));
	                   invertedIndex.add(wordId++,docId);
                    }
				
				}

			} // end looping through entries

			return docId++;
		}

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

			for (auto token : tokens) {
				queryTokens.insert(token);
			}
			
			vector<shared_ptr<Set>> intersectionSet;
			for (auto token : queryTokens) {
				vector<shared_ptr<Set>> unionSet;
				for (auto field : fields) {
					Word word(field, token);
					auto found = wordIndex.find(word);
					
					if (found != wordIndex.end()) {
						auto wordId = found->second;
						CompressedSet* docSet; 
						invertedIndex.get(wordId,docSet);
						unionSet.push_back(shared_ptr<Set>(docSet));
					}
				}
				intersectionSet.push_back(shared_ptr<Set>(new LazyOrSet(unionSet)));
			}
			LazyAndSet andSet(intersectionSet);
			auto documents = documentStore->getDocuments();
			
			
			shared_ptr<Set::Iterator> it = andSet.iterator();
			while(it->nextDoc()!= NO_MORE_DOCS) {
				cout << it->docID() << endl;
				documentSet.insert(documents[it->docID()]);
			}

			return documentSet;
		}

	private:

		unsigned long docId = 0;
		unsigned long wordId = 0;

		string queryParserDelimiters;

		// tokenizer
		shared_ptr<ITokenizer> tokenizer;

		// store all the words
		map<Word, unsigned int> wordIndex;

		// store all the fields
		set<string> fields;

		// store all the documents
		shared_ptr<IDocumentStore> documentStore;

		// inverted index that maps words(wordId) to documents that contain it
	//	map<unsigned int, set<unsigned int>> invertedIndex;
		
		InvertedIndexSimple invertedIndex;

};
