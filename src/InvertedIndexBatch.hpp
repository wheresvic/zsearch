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
#include <time.h>
#include <exception>

#include <atomic>
#include <condition_variable>
#include <chrono>
#include <thread>

#include "IInvertedIndex.h"
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "IKVStore.h"


#include "varint/ISetFactory.h"
#include "SparseSet.hpp"
#include "LRUCache.hpp"


#ifdef __MACH__
#include <sys/time.h>
#define CLOCK_REALTIME 0
#define CLOCK_MONOTONIC 0
int clock_gettime(int /*clk_id*/, struct timespec* t) {
    struct timeval now;
    int rv = gettimeofday(&now, NULL);
    if (rv) return rv;
    t->tv_sec  = now.tv_sec;
    t->tv_nsec = now.tv_usec * 1000;
    return 0;
}
#else
#include <time.h>
#endif


struct postingComp {
  inline bool operator()(const std::pair<unsigned int,unsigned int>& first,
  const std::pair<unsigned int,unsigned int>& second) const{
   return (first.second < second.second);
  }
};

// The Goal of this file is to Sort and flush the list of (wordid,docid) pairs
// each time it reaches its maximum in-memory size using KVStore batching support
// but also avoid unserilizing a dccumenSet each time we add a new docID.
class InvertedIndexBatch : public IInvertedIndex
{
	struct EvictionCallback{
	  InvertedIndexBatch* invertedIndex;
	  EvictionCallback(InvertedIndexBatch* invertedIndexp): invertedIndex(invertedIndexp){

	  }
	  void evict(unsigned int key,shared_ptr<Set> data){
		invertedIndex->evictPut(key,data);
	  }  
    };
private:
	LRUCache<unsigned int,shared_ptr<Set>,EvictionCallback> wordSetCache;		

	std::shared_ptr<KVStore::IKVStore> store;

	vector<std::pair<unsigned int,unsigned int>> postings;
	vector<std::pair<unsigned int,unsigned int>> postings2;

	atomic<vector<std::pair<unsigned int,unsigned int>>*> consumerVec;
	atomic<vector<std::pair<unsigned int,unsigned int>>*> producerVec;

	std::shared_ptr<ISetFactory> setFactory;

	int maxbatchsize;
	int minbatchsize;
	volatile int batchsize;

	// For the threading
	std::thread consumerThread;
    //leveldb::port::Mutex m;
    //leveldb::port::CondVar cond_var;
    std::mutex m;
    std::condition_variable cond_var;
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

	// Need to be synchronized
	void evictPut(unsigned int wordId, const shared_ptr<Set> set){
		stringstream ss;
		set->write(ss);
		string bitmap = ss.str();
        store->Put(wordId,bitmap);
	}


	int cachePut(unsigned int wordId, const shared_ptr<Set> set)
	{
		stringstream ss;
		set->write(ss);
		string bitmap = ss.str();
		wordSetCache.put(wordId,set);
		return 1;
	}


public:

	InvertedIndexBatch(std::shared_ptr<KVStore::IKVStore> store, shared_ptr<ISetFactory> setFactory) :
	 wordSetCache(100000,this),
	 store(store),
	 setFactory(setFactory)
	 
	{
		void consumer_main();

		maxbatchsize = 100000;
		minbatchsize = 10000;
		batchsize = 0;

		producerVec.store(&postings);
		consumerVec.store(&postings2);

		consumerThread = std::thread([this](){
			       this->consumer_main();
		        });

	}

	~InvertedIndexBatch() {
		flushBatch();
		stopConsumerThread();
		wordSetCache.evictAll();
	}

	void stopConsumerThread(){
		if(done){
			return;
		}
		m.lock();
		done=true;
		m.unlock();
		//wakup if sleeping
		cond_var.notify_all();
		//wait for termination
		consumerThread.join();
	}

	void shutDownBatchProcessor()
	{
		stopConsumerThread();
	}

	void setMaxBatchSize(unsigned int newSize)
	{
		maxbatchsize = newSize;
	}

    // we need an LRU cache her
	int get(unsigned int wordId, shared_ptr<Set>& inset) const
	{
		try
		{
			string bitmap;
			if(store->Get(wordId,bitmap).ok())
			{
				stringstream bitmapStream(bitmap);
				inset = setFactory->createSparseSet();
				inset->read(bitmapStream);
				return 1;
			}
		}
		catch (exception ex)
		{
			cerr << "get " << ex.what() << endl;
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
		if (wordSetCache.get(wordid,docSet)){
            return docSet;
		}
	  	if(!get(wordid,docSet)){
		    docSet = setFactory->createSparseSet();
		}
		return docSet;
	}


	// this is not good - if you call this from another thread that means that you'll be waiting for a while for this to finish
	void flushBatch(){
	   std::unique_lock<std::mutex> lk(m);
	   while (batchsize > maxbatchsize){
	   	// std::cout << "foreground: batchsize: " << batchsize << " > maxbatchsize: "<< maxbatchsize << std::endl;
	     cond_var.wait(lk);
	   }
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
					cachePut(wordid, docSet);
					docSet = getOrCreate(posting.second);
					wordid = posting.second;
					last = posting.first;
				}
				docSet->addDoc(posting.first);
				assert(posting.first >= last);
				last = posting.first;

			}
			cachePut(wordid, docSet);
			vec.clear();
		}
		return 1;
	}

long long nanosec_elapsed(struct timespec diff) const {
    return ((long long)diff.tv_sec * 1000000000) + diff.tv_nsec;
}

struct timespec diff_timespec(struct timespec start, struct timespec end) const {
    struct timespec result;

    if (end.tv_nsec < start.tv_nsec){ // peform carry like in normal subtraction
        //                123456789
        result.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;        
        result.tv_sec = end.tv_sec - 1 - start.tv_sec;
    }
    else{
        result.tv_nsec = end.tv_nsec - start.tv_nsec;        
        result.tv_sec = end.tv_sec - start.tv_sec;
    }

    return result;
}

void timespec_addms(struct timespec *ts, long ms) {
    int sec=ms/1000;
    ms=ms-sec*1000;

    // perform the addition
    ts->tv_nsec+=ms*1000000;

    // adjust the time
    ts->tv_sec+=ts->tv_nsec/1000000000 + sec;
    ts->tv_nsec=ts->tv_nsec%1000000000;
}
    int getBatchFromQueue(){
                // maximum time in ms spends waiting to complete a full batch
                // This doesn't limit the time spent in the queue.
                int maxDelay = 100;
 
                struct timespec now;
                clock_gettime(CLOCK_REALTIME, &now);
                struct timespec firstTime = now;
                timespec_addms(&firstTime, maxDelay);
                unsigned int localbatchSize;
                do { 
                    std::unique_lock<std::mutex> lk(m);  
                    localbatchSize = batchsize;                
                    if (localbatchSize < minbatchsize){
                        long long maxWait = nanosec_elapsed(diff_timespec(now, firstTime));
                        if ( maxWait <= 0 ){
                           break;
                        }
                        cond_var.wait_for(lk,std::chrono::milliseconds(maxDelay)); 
                    }
                    clock_gettime(CLOCK_REALTIME, &now);
                } while (localbatchSize < minbatchsize && !done);


  
		if (localbatchSize > 0){
			m.lock();
		    vector<std::pair<unsigned int,unsigned int>>* temp;
		    temp = producerVec.load();
		    producerVec.store(consumerVec.load());
		    consumerVec.store(temp);
		    batchsize = 0;
		    m.unlock();
		} 
		//std::cout<< "background: get batch of size " << localbatchSize  << std::endl;
        return localbatchSize;
    }

	void consumer_main(){
	    while (!done) {
	        if (getBatchFromQueue()){
			    // TODO: use a ThreadPoolExecutor to execute it
			    flushInBackground();
			  //  std::cout<< "background: processsed one batch" << std::endl;
	            m.lock();
	            cond_var.notify_all();
	            m.unlock();
	        }
			
	    }
	}

	int add(unsigned int wordId, unsigned int docid)
	{
		m.lock();
		producerVec.load()->push_back(std::pair<unsigned int,unsigned int>(docid,wordId));
		batchsize +=1;
		cond_var.notify_all();
		bool shouldFlush =batchsize > maxbatchsize;
		m.unlock();
		if (shouldFlush){

		    flushBatch();
		}
		return 1;
	}



	void add(unsigned int docid, const SparseSet& documentWordId)
	{
		m.lock();
		for (auto value : documentWordId)
		{
			producerVec.load()->push_back(std::pair<unsigned int, unsigned int>(docid, value));
			batchsize +=1;
		}

		cond_var.notify_all();
        bool shouldFlush =batchsize > maxbatchsize;
		m.unlock();
		if (shouldFlush){
		    flushBatch();
		}
	}
	
	// better batch add that doesnt lock and unlock for each wordid
	void add(unsigned int docid, const set<unsigned int>& documentWordId)
	{
		m.lock();
		for (auto value : documentWordId)
		{
			producerVec.load()->push_back(std::pair<unsigned int, unsigned int>(docid, value));
			batchsize +=1;
		}
		if (batchsize > minbatchsize)
		{
		    cond_var.notify_all();
        }
		m.unlock();
	}

	int remove(unsigned int wordId, unsigned int docId)
	{
		throw -21;
	}

	int Compact(){
		store->Compact();
		return 1;
	}

};

#endif
