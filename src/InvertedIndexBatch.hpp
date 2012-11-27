#ifndef INVERTED_INDEX_BATCH_H
#define INVERTED_INDEX_BATCH_H

#include "IInvertedIndex.h"
#include <memory>
#include <map>
#include <set>
#include <iostream>
#include <sstream>
#include <string>
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "IKVStore.h"
#include <sparsehash/dense_hash_map>
#include <vector>
#include <algorithm>

#include "varint/ISetFactory.h"


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

	vector<std::pair<unsigned int,unsigned int>> postings;
	std::shared_ptr<ISetFactory> setFactory;
	
	int maxbatchsize;
	int batchsize;

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
	
	int put(unsigned int wordId, const shared_ptr<Set>& set)
	{
		return 1;
	}

public:

	InvertedIndexBatch(std::shared_ptr<KVStore::IKVStore> store, shared_ptr<ISetFactory> setFactory) : store(store), setFactory(setFactory)
	{
		maxbatchsize = 500000;
		batchsize = 0;
		store->Open();

	}
	
	~InvertedIndexBatch()
	{

	   flushBatch();
	}
	

	int get(unsigned int wordId, shared_ptr<Set>& inset)
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

	int flushBatch()
	{
		if (postings.size() > 0){
			std::stable_sort(postings.begin(),postings.end(),postingComp());
			unsigned int wordid = postings[0].second;
			shared_ptr<Set> docSet = getOrCreate(wordid);
			for (auto posting : postings){
				if (posting.second != wordid){
					storePut(wordid, docSet);
					docSet = getOrCreate(posting.second);
					wordid = posting.second;
				}
				docSet->addDoc(posting.first);
			}
			storePut(wordid, docSet);
			
		}
		postings.clear();
		batchsize = 0;
		return 1;
	}
	
	int add(unsigned int wordId, unsigned int docid)
	{
		postings.push_back(std::pair<unsigned int,unsigned int>(docid,wordId));
		batchsize +=1;
		if(batchsize > maxbatchsize){
	//		flushBatch();
		}
		return 1;
	}
	
	
	int Compact(){
	//	store->Compact();
		return 1;
	}
	
};

#endif