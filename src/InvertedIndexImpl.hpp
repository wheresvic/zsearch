#ifndef INVERTED_INDEX_IMPL_H
#define INVERTED_INDEX_IMPL_H

#include <memory>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include "varint/Set.h"
#include "varint/CompressedSet.h"
#include "varint/BasicSet.h"
#include "IKVStore.h"
#include "IInvertedIndex.h"

template <class SET>
class InvertedIndexImpl : public IInvertedIndex<SET>
{
	private:

		std::shared_ptr<KVStore::IKVStore> store;		
	
	public:

		InvertedIndexImpl(std::shared_ptr<KVStore::IKVStore> store) : store(store)
		{
			store->Open();
		}

		int get(unsigned int wordId, shared_ptr<SET>& outset)
		{
			string bitmap;
			if(store->Get(wordId,bitmap).ok())
			{ 
				outset = make_shared<SET>();
				stringstream bitmapStream(bitmap);
				outset->read(bitmapStream);
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


		int put(unsigned int wordId, SET& set)
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

		int add(unsigned int wordId, unsigned int docid)
		{
			if (exist(wordId))
			{
				shared_ptr<SET> set;
				get(wordId,set);
				if (!set->find(docid)){
					set->addDoc(docid);
					put(wordId,*set);
				}
			}
			else
			{
				SET set;
				set.addDoc(docid);
				put(wordId,set);
			}

			return 1;
		}
	
};

#endif
