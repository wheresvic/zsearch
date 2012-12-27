#ifndef INVERTED_INDEX_BATCH_H
#define INVERTED_INDEX_BATCH_H

#include <memory>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

#include <atomic>
#include "port_posix.h"

#include <thread>

#include "IInvertedIndex.h"
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "IKVStore.h"
#include <sparsehash/dense_hash_map>


#include "varint/ISetFactory.h"
#include "KVStoreLevelDBBatch.h"

using google::dense_hash_map;

struct postingComp {
  inline bool operator()(const std::pair<unsigned int,unsigned int>& first,
  const std::pair<unsigned int,unsigned int>& second) const{
   return (first.second < second.second);
  }
};

class InvertedIndexBatch : public IInvertedIndex
{
private:
	std::shared_ptr<KVStore::IKVStore> store;
	KVStore::KVStoreLevelDBBatch batch;

	vector<std::pair<unsigned int,unsigned int>> postings;
	vector<std::pair<unsigned int,unsigned int>> postings2;

	atomic<vector<std::pair<unsigned int,unsigned int>>*> consumerVec;
	atomic<vector<std::pair<unsigned int,unsigned int>>*> producerVec;

	std::shared_ptr<ISetFactory> setFactory;

	int maxbatchsize;
	volatile int batchsize;

	// For the threading
	std::thread consumerThread;
    leveldb::port::Mutex m;
    leveldb::port::CondVar cond_var;
	volatile bool done = false;


	int storePut(unsigned int wordId, const shared_ptr<Set> set)
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

	int batchPut(unsigned int wordId, const shared_ptr<Set> set)
	{
		stringstream ss;
		set->write(ss);
		string bitmap = ss.str();

		batch.Put(wordId,bitmap);
		return 1;
	}


public:

	InvertedIndexBatch(std::shared_ptr<KVStore::IKVStore> store, shared_ptr<ISetFactory> setFactory) :
	 store(store),
	 setFactory(setFactory),
	 cond_var(&m)
	{
		void consumer_main();

		maxbatchsize = 2500000;
		batchsize = 0;
		store->Open();

		producerVec.store(&postings);
		consumerVec.store(&postings2);
		consumerThread = std::thread([this](){
			       this->consumer_main();
		        });

	}

	~InvertedIndexBatch() {
		flushBatch();
		stopConsumerThread();
	}

	void stopConsumerThread(){
		done=true;
		//wakup if sleeping
		cond_var.Signal();
		//wait for termination
		consumerThread.join();
	}

	int get(unsigned int wordId, shared_ptr<Set>& inset) const
	{
		string bitmap;
		if(store->Get(wordId,bitmap).ok())
		{
			stringstream bitmapStream(bitmap);
			inset = setFactory->createSparseSet();
			inset->read(bitmapStream);
			return 1;
		}
		return 0;
	}

	bool exist(unsigned int wordId)
	{
		string ret;
		bool found = store->Get(wordId,ret).ok();
		return found;
	}

	shared_ptr<Set> getOrCreate(unsigned int wordid){
		shared_ptr<Set> docSet;
	  	if(!get(wordid,docSet)){
		    docSet = setFactory->createSparseSet();
		}
		return docSet;
	}


	// this is not good - if you call this from another thread that means that you'll be waiting for a while for this to finish
	void flushBatch(){
	   m.Lock();
	   while (batchsize > 0){
	     cond_var.Wait();
	   }
	   m.Unlock();
	}



	int flushInBackground()
	{
		vector<std::pair<unsigned int,unsigned int>>& vec = *consumerVec.load();
		if (vec.size() > 0){
			std::stable_sort(vec.begin(),vec.end(),postingComp());

			unsigned int wordid = vec[0].second;
			shared_ptr<Set> docSet = getOrCreate(wordid);
			unsigned int last = vec[0].first;
			for (auto posting : vec){
				if (posting.second != wordid){
					batchPut(wordid, docSet);
					docSet = getOrCreate(posting.second);
					wordid = posting.second;
					last = posting.first;
				}
				docSet->addDoc(posting.first);
				assert(posting.first >= last);
				last = posting.first;

			}
			batchPut(wordid, docSet);
			vec.clear();
		}

		// store->Write(batch);
		batch.Clear();
		return 1;
	}

	void consumer_main(){
	    while (!done) {
		    m.Lock();
		    if (batchsize > 0){
				vector<std::pair<unsigned int,unsigned int>>* temp;
				temp = producerVec.load();
				producerVec.store(consumerVec.load());
				consumerVec.store(temp);
				batchsize = 0;
				cond_var.Signal();
		    } else {
			   // sleep
			   while (batchsize == 0 && !done){
			     cond_var.Wait();
			   }
		    }
	        m.Unlock();
			flushInBackground();
	    }
	}

	int add(unsigned int wordId, unsigned int docid)
	{
		m.Lock();
		producerVec.load()->push_back(std::pair<unsigned int,unsigned int>(docid,wordId));
		batchsize +=1;
		cond_var.Signal();
		m.Unlock();
		return 1;
	}


	int Compact(){
		store->Compact();
		return 1;
	}

};

#endif
