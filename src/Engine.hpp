
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
#include "InvertedIndexImpl.hpp"
#include "varint/CompressedSet.h"
#include "varint/LazyOrSet.h"
#include "varint/LazyAndSet.h"
#include "IKVStore.h"
#include "WordIndex.hpp"
#include "varint/ISetFactory.h"

using namespace std;

class Engine
{
	public:

		Engine(shared_ptr<ITokenizer> tokenizer,
				shared_ptr<IDocumentStore> documentStore,
				shared_ptr<KVStore::IKVStore> invertedIndexStore,
				shared_ptr<ISetFactory> setFactory) :
			tokenizer(tokenizer),
			documentStore(documentStore),
			invertedIndex(invertedIndexStore, setFactory),
			setFactory(setFactory)
		{ 
			
		}

        void flushBatch(){
			invertedIndex.flushBatch();
        }

        ~Engine(){
        }

		unsigned int addDocument(const shared_ptr<IDocument>& document)
		{
			documentStore->addDoc(docId, document);
			// we create a set of word in the document
			// to avoid duplicate pair<wordid,docid>
            set<unsigned int> documentWordId;
			auto& entries = document->getEntries();

			for (auto iter = entries.begin(); iter != entries.end(); ++iter)
			{
				const string& field = iter->first;
				const string& value = iter->second;

				fields.insert(field);

				tokenizer->setString(value);
				while (tokenizer->nextToken())
				{
				    unsigned int id = 0;
				    const string& token = tokenizer->getToken();

                    if(wordIndex.Get(field,token,id)){
	                   
						documentWordId.insert(id);
					} else {
					   	wordIndex.Put(field,token,wordId);
					    documentWordId.insert(wordId++);
					}

				}
			} // end looping through entries
			for (auto value : documentWordId){
			//  if (value == 197){
			//	std::cout << "adding doc: "<< docId << std::endl;	
			//  }	
			  invertedIndex.add(value,docId);	
			} 
			return docId++;
		}


		/**
		 * Return documents that contain all the words in the query
		 */
		set<unsigned int> search(const string& query)
		{
			// q = "", field1="some more text" = input1/some input1/more input1/text
			// q = "some more text" = (input1/some OR input2/some ) AND (input1/more or input2/more)

			set<unsigned int> documentIdSet;

			set<string> queryTokens;

			tokenizer->setString(query);
			while (tokenizer->nextToken())
			{
				queryTokens.insert(tokenizer->getToken());
			}

			vector<shared_ptr<Set>> intersectionSet;

			for (auto token : queryTokens) 
			{
				vector<shared_ptr<Set>> unionSet;
				
				for (auto field : fields) 
				{					
					unsigned int wordId = 0;
                    
					if(wordIndex.Get(field,token,wordId))
					{
						std::cout << "field: "<< field <<" token: " << "wordid: "<< wordId << std::endl;
						shared_ptr<Set> docSet;
						invertedIndex.get(wordId,docSet);	
						unionSet.push_back(docSet);
					}
				}
				
				shared_ptr<Set> orset = make_shared<LazyOrSet>(unionSet);
				intersectionSet.push_back(orset);
			}
			
			LazyAndSet andSet(intersectionSet);
			shared_ptr<Set::Iterator> it = andSet.iterator();
			
			while(it->nextDoc() != NO_MORE_DOCS) 
			{
				documentIdSet.insert(it->docID());
			}

			return documentIdSet;
		}

		/**
		 * Return documents given their docIds
		 */
		set<shared_ptr<IDocument>> getDocs(const set<unsigned int>& docIds)
		{
			set<shared_ptr<IDocument>> documentSet;
		
			for (auto docId : docIds)
			{
				shared_ptr<IDocument> doc;
				
				// if (documentStore->Get(docId, doc))
				if (getDoc(docId, doc))
				{
					documentSet.insert(doc);
				}
			}
			
			return documentSet;
		}
		
		bool getDoc(const unsigned int docId, shared_ptr<IDocument>& doc)
		{
			if (documentStore->Get(docId, doc))
			{
				return true;
			}
		
			return false;
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
		InvertedIndexBatch invertedIndex;
		
		shared_ptr<ISetFactory> setFactory;
};
