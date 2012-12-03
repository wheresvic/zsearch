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
	
	std::shared_ptr<ISetFactory> setFactory;
	
	int maxbatchsize;
	int batchsize;
	
	// For the threading
	std::thread consumerThread;
    leveldb::port::Mutex m;
    leveldb::port::CondVar cond_var;
    leveldb::port::Mutex m2;
    leveldb::port::CondVar cond_var2;
	volatile bool done = false;
	volatile bool notified = false;

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
	 cond_var(&m),
	 cond_var2(&m2)
	{
		void consumer_main();
		maxbatchsize = 2500000;
		batchsize = 0;
		store->Open();
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
		notified = true;
		cond_var.Signal();
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
	


	void flushBatchAsync(){
		if (notified != true ){
			std::swap(postings,postings2);
			batchsize = 0;
			notified = true;
			cond_var.Signal();
		} 
	}
	
	void flushBatch(){
		//wait for current background task to finish
		m2.Lock();
        while (notified) {  // loop to avoid spurious wakeups
           cond_var2.Wait();
        }
        m2.Unlock();
	    if (batchsize >0){
		    //flush remaining
			flushBatchAsync();
	    }
		m2.Lock();
        while (notified) {  // loop to avoid spurious wakeups
           cond_var2.Wait();
        }
		m2.Unlock();
	}
	
	int flushInBackground()
	{
		if (postings2.size() > 0){
			std::stable_sort(postings2.begin(),postings2.end(),postingComp());
			unsigned int wordid = postings2[0].second;
			shared_ptr<Set> docSet = getOrCreate(wordid);
			for (auto posting : postings2){
				if (posting.second != wordid){
					batchPut(wordid, docSet);
					docSet = getOrCreate(posting.second);
					wordid = posting.second;
				}
				docSet->addDoc(posting.first);
			}
			batchPut(wordid, docSet);
		}
		postings2.clear();
		store->Write(batch);
		batch.Clear();
		return 1;
	}
	
	void consumer_main(){
		m.Lock();
	    while (!done) {
	        while (!notified) {  // loop to avoid spurious wakeups
	           cond_var.Wait();
	        }   
			flushInBackground();
			notified = false;
			cond_var2.Signal();
	    }
	    m.Unlock();
	}
	
	int add(unsigned int wordId, unsigned int docid)
	{
		postings.push_back(std::pair<unsigned int,unsigned int>(docid,wordId));
		//TODO: we need to track memory usage instead 
		batchsize +=1;
		if(batchsize > maxbatchsize){
			flushBatchAsync();
		}
		return 1;
	}
	
	
	int Compact(){
	//	store->Compact();
		return 1;
	}
	
};

#endif