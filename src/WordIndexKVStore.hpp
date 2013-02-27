#ifndef WORDINDEXKVSTORE_H
#define WORDINDEXKVSTORE_H

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "IKVStore.h"
#include "ZException.hpp"
#include "Word.hpp"
#include "ZUtil.hpp"
#include "LRUCache.hpp"
using namespace std;

class WordIndexKVStore
{
	private:
		mutable LRUCache<std::string,unsigned int> cache;
		std::shared_ptr<KVStore::IKVStore> store;

	public:

		WordIndexKVStore(std::shared_ptr<KVStore::IKVStore> store) : cache(50000),store(store)
		{
			// store->Open();
		}
		
		~WordIndexKVStore()
		{
			std::cerr << "Destroyed WordIndexKVStore" << std::endl;
		}

		int Put(const std::string& field, const std::string& token, unsigned int value)
		{
			Word word(field, token);
			return Put(word, value);
		}

		int Get(const std::string& field, const std::string& token, unsigned int& value) const
		{
			Word word(field, token);
			
			return Get(word, value);
		}
		
		int Put(const Word& word, unsigned int value)
		{
			const std::string wordString =word.toString();
			cache.put(wordString,value);
			
			string v = ZUtil::getString(value);
			
			//TODO: this need to be batched too 
			if (store->Put(wordString, v).ok())
			{
				return 1;
			}
			
			return 1;
			return 0;
		}
		
        
		int Get(const Word& word, unsigned int& value) const
		{
			const std::string wordString =word.toString();
			
			if (cache.exist(wordString)){
				value = cache.get(wordString);
				return 1;
			} 
					
			string v;
			if (store->Get(wordString, v).ok())
			{
				value = ZUtil::getUInt(v);
				cache.put(wordString,value);
				return 1;
			}

			return 0;
		}		
};

#endif // WORDINDEX_H
