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
#include "varint/SetFactory.h"

using google::dense_hash_map;
// TODO implement ConcurrentMerge

class InvertedIndexBatch : public IInvertedIndex
{
private:

	std::shared_ptr<KVStore::IKVStore> store;
	dense_hash_map<unsigned int, shared_ptr<vector<unsigned int>>> buffer;
	SetFactory setFactory;
	const SetType setType;
	
	int maxbatchsize;
	int batchsize;

	int storePut(unsigned int wordId, const shared_ptr<Set>& set)
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
	
	InvertedIndexBatch(std::shared_ptr<KVStore::IKVStore> store, SetType setType) :
	 store(store), 
	 buffer(656538),
	 setType(setType)
	{
		maxbatchsize = 200000000;
		batchsize = 0;
		store->Open();
		buffer.set_empty_key(0);
	}
	~InvertedIndexBatch()
	{

	}
	

	int get(unsigned int wordId, shared_ptr<Set>& inset)
	{		
		string bitmap;
		if(store->Get(wordId,bitmap).ok())
		{
			stringstream bitmapStream(bitmap);
			inset = setFactory.createSet(setType);
			inset->read(bitmapStream);
			return 1;
		}
		// search inside the batch buffer
		auto iter = buffer.find(wordId);
		if (iter != buffer.end())
		{
			inset = setFactory.createSet(setType);
			for (auto docid = iter->second->begin(); docid != iter->second->end(); ++docid){	
			   inset->addDoc(*docid);
			}
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
	
	int flushBatch()
	{
		for (auto iter = buffer.begin(); iter != buffer.end(); ++iter)
        {
            shared_ptr<Set> docSet;
			if(!get(iter->first,docSet)){
			    docSet = setFactory.createSet(setType);	
			}
			//use a normal set to remove duplicate document
			set<unsigned int> docBatch;
			for (auto docid : *(iter->second)){
				docBatch.insert(docid);	
			}
			//add all elements of the set to our compressed Set
			for (auto docid : docBatch) {
				docSet->addDoc(docid);
			}
			storePut(iter->first, docSet);		
        }
		buffer.clear();
		batchsize = 0;
		return 1;
	}
	
	int add(unsigned int wordId, unsigned int docid)
	{
		//todo: keep track of memory usage and # of doc in batch
		batchsize +=1;
		auto iter = buffer.find(wordId);
		
		if (iter!= buffer.end()){
			iter->second->push_back(docid);
		} else {
			shared_ptr<vector<unsigned int>> v = make_shared<vector<unsigned int>>();
			v->push_back(docid);
			buffer.insert(std::make_pair(wordId, v));
		}
		if(batchsize > maxbatchsize){
			flushBatch();
		}
		return 1;
	}
	
	
	int Compact(){
	//	store.Compact();
		return 1;
	}
};

#endif