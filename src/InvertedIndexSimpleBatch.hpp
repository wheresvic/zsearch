#ifndef INVERTED_INDEX_SIMPLE_BATCH_H
#define INVERTED_INDEX_SIMPLE_BATCH_H

#include <memory>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

#include "varint/ISetFactory.h"
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "varint/BasicSet.h"

#include "IKVStore.h"
#include "IInvertedIndex.h"

/*
struct postingComp
{
	inline bool operator()(const std::pair<unsigned int, unsigned int>& first,
							const std::pair<unsigned int, unsigned int>& second) const
	{
		return (first.second < second.second);
	}
};
*/

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

		std::atomic<unsigned int> maxBatchSize;
	    std::atomic<bool> done;
	    std::atomic<bool> dataReady;
	    std::mutex m;
		std::condition_variable cond;

		std::thread batchProcessor;

		unsigned int batchSize; 	// intentionally left naked so that it is always processed under lock
	    std::map<unsigned int, std::set<unsigned int>> postings;

	protected:

		/**
		 * Used by the thread processor
		 *
		 * wordId -> set(docIds)
		 *
		 * loop through all the items, check if wordId exists in the store
		 * if yes, then get the sparseset, add all the docIds to it
		 * (since we're using a set, they should be ordered)
		 * if no, then create own sparseset
		 *
		 * finally insert in batch
		 */
		int flush(const std::map<unsigned int, std::set<unsigned int>>& postings)
		{
			// std::stable_sort(postings.begin(), postings.end(), postingComp());

			unsigned int processed = 0;

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

						++processed;
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
						++processed;
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
				cout << "wrote " << processed << " postings" << endl;
				return 1;
			}

			return 0;
		}

		/**
		 * loop and wait on lock until batch size has been hit
		 *
		 * once woken up and data ready, we can process it by taking a
		 * copy under lock, clear the original, release the lock
		 * and flush the batch
		 */
		void processBatch()
		{
			cout << "starting batch processor" << endl;

			while(!done)
			{
				unique_lock<mutex> lock(m);

				while(!dataReady) // in case you get woken up and batch size is not hit
				{
					cout << "waiting for data" << endl;
					cond.wait(lock);
				}

				// process data, if shutdown called then we will flush the batch manually
				// this is because if shutdown is called when batch is being flushed
				// then any extra postings wont be flushed
				//
				// whether we really care about these is another story

				if (!done)
				{
					cout << "processing batchSize " << batchSize << endl;

					map<unsigned int, set<unsigned int>> postingsCopy(postings);

					postings.clear();
					batchSize = 0;
					dataReady = false;

					lock.unlock();

					flush(postingsCopy);
				}
			}

			cout << "batch processor done" << endl;
		}

	public:

		InvertedIndexSimpleBatch(std::shared_ptr<KVStore::IKVStore> store, shared_ptr<ISetFactory> setFactory) : store(store), setFactory(setFactory), maxBatchSize(5), batchSize(0)
		{
			done = false;
			dataReady = false;
			store->Open();

			batchProcessor = std::thread([this]()
			{
				this->processBatch();
			});
		}

		~InvertedIndexSimpleBatch()
		{
			shutDownBatchProcessor();

			flush(postings);
		}

		void setMaxBatchSize(unsigned int newSize)
		{
			maxBatchSize = newSize;
		}

		/**
		 * Perhaps we shouldn't be exposing this method
		 */
		void flushBatch()
		{
			unique_lock<mutex> lock(m);

			flush(postings);

			batchSize = 0;
			postings.clear();
		}

		/**
		 * Perhaps we shouldn't be exposing this method
		 */
		void shutDownBatchProcessor()
		{
			if (!done)
			{
				// wake up and kill the thread
				dataReady = true;
				done = true;
				cond.notify_one();

				batchProcessor.join();
			}
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
			// cout << "adding item" << endl;

			unique_lock<mutex> lock(m);

			++batchSize;

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

			if (batchSize >= maxBatchSize)
			{
				if (!done)
				{
					// cout << "notifying batch processor" << endl;

					dataReady = true;
					cond.notify_one();
				}
				else
				{
					cout << "batch processor not running, flushing batch in place" << endl;

					int result = flush(postings);

					batchSize = 0;
					postings.clear();

					return result;
				}
			}

			return 1;
		}

		int remove(unsigned int wordId, unsigned int docId)
		{
			int ret = 0;

			unique_lock<mutex> lock(m);

			auto iter = postings.find(wordId);

			if (iter != postings.end())
			{
				(iter->second).erase(docId);
				ret = 1;
			}

			// is it possible that the same wordId, docId combo also exists in the index?

			lock.unlock();

			shared_ptr<Set> set;

			if (get(wordId, set))
			{
				set->removeDocId(docId);
				ret = put(wordId, set);
			}

			return ret;
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


};

blabla

#endif
