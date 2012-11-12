
#include <string>
#include <map>
#include "DocumentStoreSimple.h"
#include <memory>
// #include <utility>
#include "QueryParser.hpp"
#include <set>
#include <vector>
#include "Word.hpp"
#include "InvertedIndexBatch.hpp"
#include "varint/CompressedSet.h"
#include "varint/LazyOrSet.h"
#include "varint/LazyAndSet.h"
#include "IKVStore.h"
#include "WordIndex.hpp"

using namespace std;

template <class SET>
class Engine
{
	public:

		Engine(shared_ptr<ITokenizer> tokenizer,
				shared_ptr<IDocumentStore> documentStore,
				shared_ptr<KVStore::IKVStore> invertedIndexStore) :
			tokenizer(tokenizer),
			documentStore(documentStore),
			invertedIndex(invertedIndexStore)
		{ 
			
		}

        void flushBatch(){
			invertedIndex.flushBatch();
        }

        ~Engine(){
			flushBatch();
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

				tokenizer->setString(value);
				while (tokenizer->nextToken())
				{
				    unsigned int id = 0;
				    string token = tokenizer->getToken();
                    if(wordIndex.Get(field,token,id)){
					    invertedIndex.add(id,docId);
					} else {
					   	wordIndex.Put(field,token,wordId);
					   	invertedIndex.add(wordId++,docId);
					}

				}

			} // end looping through entries

			return docId++;
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


			tokenizer->setString(query);
			while (tokenizer->nextToken())
			{
				queryTokens.insert(tokenizer->getToken());
			}

			vector<shared_ptr<Set>> intersectionSet;
			for (auto token : queryTokens) {
				vector<shared_ptr<Set>> unionSet;
				for (auto field : fields) {					
					unsigned int wordId = 0;
                    if(wordIndex.Get(field,token,wordId)){
					    shared_ptr<SET> docSet;
						invertedIndex.get(wordId,docSet);
						unionSet.push_back(docSet);
					}
				}
				intersectionSet.push_back(shared_ptr<Set>(new LazyOrSet(unionSet)));
			}
			LazyAndSet andSet(intersectionSet);
			auto documents = documentStore->getDocuments();


			shared_ptr<Set::Iterator> it = andSet.iterator();
			while(it->nextDoc()!= NO_MORE_DOCS) {
				documentSet.insert(documents[it->docID()]);
			}

			return documentSet;
		}

	private:

		unsigned long docId = 1;
		unsigned long wordId = 1;

		string queryParserDelimiters;

		// tokenizer
		shared_ptr<ITokenizer> tokenizer;

		// store all the words
		WordIndex wordIndex;

		// store all the fields
		set<string> fields;

		// store all the documents
		shared_ptr<IDocumentStore> documentStore;

		// inverted index that maps words(wordId) to documents that contain it
		InvertedIndexBatch<SET> invertedIndex;

};
