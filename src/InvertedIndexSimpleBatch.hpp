#ifndef INVERTED_INDEX_SIMPLE_BATCH_H
#define INVERTED_INDEX_SIMPLE_BATCH_H

#include <memory>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "varint/ISetFactory.h"
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "varint/BasicSet.h"

#include "IKVStore.h"
#include "IInvertedIndex.h"

struct postingComp
{
	inline bool operator()(const std::pair<unsigned int, unsigned int>& first,
							const std::pair<unsigned int, unsigned int>& second) const
	{
		return (first.second < second.second);
	}
};


/**
 * The general strategy here is to add wordId, docId into a
 * map which represents the inverted index
 *
 * When we hit the max batch size, we simply grab the wordId
 * from the store - if it exists we can add all the docIds to
 * the existing set and if it doesn't we create a new set
 * and dump stuff in there
 *
 * With threading enabled, we'll simply have a thread that
 * takes over the flushing
 */
class InvertedIndexSimpleBatch : public IInvertedIndex
{
	private:

		std::shared_ptr<KVStore::IKVStore> store;
	    shared_ptr<ISetFactory> setFactory;

	    int batchSize, maxBatchSize;

	    volatile bool done = false;

	    std::map<unsigned int, std::set<unsigned int>> postings;

	public:

		InvertedIndexSimpleBatch(std::shared_ptr<KVStore::IKVStore> store, shared_ptr<ISetFactory> setFactory) : store(store), setFactory(setFactory), batchSize(0), maxBatchSize(5)
		{
			store->Open();
		}

		int get(unsigned int wordId, shared_ptr<Set>& outset) const
		{
			string bitmap;

			if(store->Get(wordId, bitmap).ok())
			{
				outset = setFactory->createSparseSet();
				stringstream bitmapStream(bitmap);
				outset->read(bitmapStream);
				return 1;
			}

			return 0;
		}



		int add(unsigned int wordId, unsigned int docid)
		{
			auto iter = postings.find(wordId);

			if (iter != postings.end())
			{
				(iter->second).insert(docid);
			}
			else
			{
				set<unsigned int> docIds;
				docIds.insert(docid);
				postings.insert(make_pair(wordId, docIds));
			}

			++batchSize;

			if (batchSize >= maxBatchSize)
			{
				int result = flushBatch(postings);
				postings.clear();
				return result;
			}

			return 1;
		}


		bool exist(unsigned int wordId)
		{
			string ret;
			bool found = store->Get(wordId,ret).ok();
			return found;
		}


		int put(unsigned int wordId, const shared_ptr<Set>& set)
		{
			stringstream ss;
			set->write(ss);
			string bitmap = ss.str();
			if (store->Put(wordId,bitmap).ok())
			{
				return 1;
			}

			return 0;
		}

		/**
		 * Take a copy of the data
		 */
		int flushBatch(std::map<unsigned int, std::set<unsigned int>> postings)
		{
			// std::stable_sort(postings.begin(), postings.end(), postingComp());

			cout << "flushing batch" << endl;

			std::vector<std::pair<unsigned int, std::string>> writes;

			for (auto posting : postings)
			{
				auto wordId = posting.first;
				auto docIds = posting.second;

				if (exist(wordId))
				{
					shared_ptr<Set> set;
					get(wordId, set);

					for (auto docid : docIds)
					{
						if (!set->find(docid))
						{
							set->addDoc(docid);
						}
					}

					// put(wordId, set);

					stringstream ss;
					set->write(ss);
					string bitmap = ss.str();

					writes.push_back(make_pair(wordId, bitmap));
				}
				else
				{
					shared_ptr<Set> set = setFactory->createSparseSet();

					for (auto docid : docIds)
					{
						set->addDoc(docid);
					}

					stringstream ss;
					set->write(ss);
					string bitmap = ss.str();

					// put(wordId, set);

					writes.push_back(make_pair(wordId, bitmap));
				}

			}

			if (store->Put(writes).ok())
			{
				return 1;
			}

			return 0;
		}


};

#endif
