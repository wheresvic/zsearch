
#include "InvertedIndexImpl.h"
#include <memory>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "KVStoreLevelDb.hpp"
#include "KVStoreInMemory.hpp"
#include "IKVStore.h"

	InvertedIndexImpl::InvertedIndexImpl(std::shared_ptr<KVStore::IKVStore> store) : store(store)
	{
		store->Open();
	}

	int InvertedIndexImpl::get(unsigned int wordId, CompressedSet*& set)
	{
		string bitmap;
		if(store->Get(wordId,bitmap).ok())
		{
			stringstream bitmapStream(bitmap);
			set = new CompressedSet();
			set->read(bitmapStream);
			return 1;
		}

		return 0;
	}

	bool InvertedIndexImpl::exist(unsigned int wordId)
	{
		string ret;
		bool found = store->Get(wordId,ret).ok();
		return found;
	}


	int InvertedIndexImpl::put(unsigned int wordId, CompressedSet& set)
	{
		stringstream ss;
		set.write(ss);
		string bitmap = ss.str();

		if (store->Put(wordId,bitmap).ok())
		{
			return 1;
		}

		return 0;
	}

	int InvertedIndexImpl::add(unsigned int wordId, unsigned int docid)
	{
		if (exist(wordId))
		{
			CompressedSet *set;
			get(wordId,set);
			if (!set->find(docid)){
				set->addDoc(docid);
				put(wordId,*set);
			}
			delete set;
		}
		else
		{
			CompressedSet set;
			set.addDoc(docid);
			put(wordId,set);
		}

		return 1;
	}
