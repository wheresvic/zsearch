
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <set>
#include <unordered_set>
#include <vector>
#include <chrono>

#include "ITokenizer.h"
#include "Word.hpp"
#include "DocumentImpl.hpp"
#include "InvertedIndexBatch.hpp"
#include "InvertedIndexSimpleBatch.hpp"
#include "InvertedIndexImpl.hpp"
#include "varint/ISetFactory.h"
#include "varint/CompressedSet.h"
#include "varint/LazyOrSet.h"
#include "varint/LazyAndSet.h"
#include "IKVStore.h"
#include "WordIndexKVStore.hpp"
#include "DocumentKVStore.hpp"
#include "FieldKVStore.hpp"
#include "EngineDataKVStore.hpp"
#include "SparseSet.hpp"

using namespace std;

class Engine
{
	public:

		Engine(shared_ptr<KVStore::IKVStore> engineDataStore,
				shared_ptr<ITokenizer> tokenizer,
				shared_ptr<KVStore::IKVStore> fieldStore,
				shared_ptr<KVStore::IKVStore> documentStore,
				shared_ptr<KVStore::IKVStore> wordIndexStore,
				shared_ptr<KVStore::IKVStore> invertedIndexStore,
				shared_ptr<ISetFactory> setFactory) :
			engineData(engineDataStore),
			tokenizer(tokenizer),
			fields(fieldStore),
			documents(documentStore),
			wordIndex(wordIndexStore),
			invertedIndex(invertedIndexStore, setFactory),
			setFactory(setFactory)
		{

		}

		/**
		 *
		 */
		void disableBatching()
		{
			invertedIndex.shutDownBatchProcessor();
		}

		/**
		 *
		 */
		void flushBatch()
        {
			invertedIndex.flushBatch();
        }

		/**
		 *
		 */
        void setMaxBatchSize(unsigned int newSize)
        {
			invertedIndex.setMaxBatchSize(newSize);
		}

		
        void Compact()
        {
	         invertedIndex.Compact();
        }
		

        ~Engine()
        {
			std::cerr << "Destroyed engine" << std::endl;
		}

		/**
		 * @param	document
		 * @return				docId of newly inserted document
		 */
		unsigned int addDocument(const shared_ptr<IDocument>& document)
		{
			sparseset.clear();
			
			//TODO: this need to be batched too 
			documents.addDoc(engineData.getDocId(), document);

			auto& entries = document->getEntries();

			for (auto iter = entries.begin(); iter != entries.end(); ++iter)
			{
				const string& field = iter->first;
				const string& value = iter->second;

				fields.put(field);

				tokenizer->setString(value);
				while (tokenizer->nextToken())
				{
				    unsigned int id = 0;
				    const string& token = tokenizer->getToken();

                    if (wordIndex.Get(field, token, id))
                    {
						sparseset.insert(id);
					}
					else
					{
					   	wordIndex.Put(field, token, engineData.getWordId());
					    sparseset.insert(engineData.getWordId()++);
					}
				}

			} // end looping through entries

            invertedIndex.add(engineData.getDocId(), sparseset);
			return engineData.getDocId()++;
		}

		/**
		 * @param 	docId
		 */
		void deleteDocument(unsigned int docId)
		{
			// TODO add exists method to remove DocumentImpl dependency
			shared_ptr<IDocument> document = make_shared<DocumentImpl>();

			// get the document to be deleted
			if (!documents.Get(docId, document))
			{
				return;
			}

			// we create a set of word in the document
			// to avoid duplicate pair<wordid,docid>
			set<unsigned int> documentWordId;

			auto& entries = document->getEntries();

			for (auto iter = entries.begin(); iter != entries.end(); ++iter)
			{
				const string& field = iter->first;
				const string& value = iter->second;

				tokenizer->setString(value);
				while (tokenizer->nextToken())
				{
				    unsigned int id = 0;
				    const string& token = tokenizer->getToken();

                    if (wordIndex.Get(field, token, id))
                    {
						documentWordId.insert(id);
					}
					else
					{
					   	// TODO: this is a very weird case ...
					}
				}


			} // end looping through entries

			for (auto value : documentWordId)
			{
				invertedIndex.remove(value, docId);
			}

			documents.removeDoc(docId);
		}


		/**
		 * @param	query
		 * @param	start
		 * @param	offset
		 * @return			a vector of documentIds that contain all the words in the query
		 */
		vector<unsigned int> search(const string& query, unsigned int start, unsigned int offset) const
		{
			// q = "some more text", field = "input1" => input1/some input1/more input1/text
			// q = "some more", field = "" => (input1/some OR input2/some ) AND (input1/more or input2/more)

			unsigned int skip = start * offset;

			vector<unsigned int> documentIdList;

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

				for (auto field : fields.getFields())
				{
					unsigned int wordId = 0;

					if (wordIndex.Get(field, token, wordId))
					{
						// cout << "field: " << field << " token: " << token << " wordid: " << wordId << endl;
						shared_ptr<Set> docSet;

						// it is possible that the entry may not have been flushed to the index
						if (invertedIndex.get(wordId, docSet))
						{
							// cout << "found" << endl;
							unionSet.push_back(docSet);
						}
					}
				}

				shared_ptr<Set> orset = make_shared<LazyOrSet>(unionSet);
				intersectionSet.push_back(orset);
			}

			LazyAndSet andSet(intersectionSet);

			//We now create a copy of andSet the fun of it ???

			shared_ptr<Set::Iterator> it = andSet.iterator();

			unsigned int i = 0;

			while(it->nextDoc() != NO_MORE_DOCS)
			{
				// cout << i << " ";

				if (i < skip)
				{
					++i;
					continue;
				}

				// if we made it here then we are past skipping
				// check whether we have a valid offset

				if (offset > 0)
				{
					// check whether we are within offset
					if ((i - skip) < offset)
					{
						++i;
						documentIdList.push_back(it->docID());
					}
					else // if we made it past offset then we can stop
					{
						break;
					}
				}
				else // if no valid offset, then we just keep adding!
				{
					documentIdList.push_back(it->docID());
				}
			}

			return documentIdList;

		}

		/**
		 * Return documents given their docIds
		 */
		vector<shared_ptr<IDocument>> getDocs(const vector<unsigned int>& docIds) const
		{
			vector<shared_ptr<IDocument>> documentSet;

			for (auto docId : docIds) {
				shared_ptr<IDocument> doc;

				if (getDoc(docId, doc)) {
					documentSet.push_back(doc);
				}
			}

			return documentSet;
		}

		bool getDoc(const unsigned int docId, shared_ptr<IDocument>& doc) const
		{
			if (documents.Get(docId, doc)) {
				return true;
			}

			return false;
		}

	private:

		EngineDataKVStore engineData;

		// tokenizer
		shared_ptr<ITokenizer> tokenizer;

		// store all the fields
		FieldKVStore fields;

		// store all the documents
		DocumentKVStore documents;

		// store all the words
		WordIndexKVStore wordIndex;

		// inverted index that maps words(wordId) to documents that contain it
		InvertedIndexBatch invertedIndex;
		// InvertedIndexImpl invertedIndex;
		// InvertedIndexSimpleBatch invertedIndex;

		// which type of set to use
		shared_ptr<ISetFactory> setFactory;
		
		SparseSet sparseset;
};
