#ifndef WORDINDEXKVSTORE_H
#define WORDINDEXKVSTORE_H

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "IKVStore.h"
#include "ZException.hpp"
#include "ZUtil.hpp"
#include "LRUCache.hpp"
using namespace std;

class WordIndexKVStore
{
	private:
		mutable LRUCache cache;
		
		std::shared_ptr<KVStore::IKVStore> store;

	public:

		WordIndexKVStore(std::shared_ptr<KVStore::IKVStore> store) : cache(656538),store(store)
		{
			// store->Open();
		}
		
		~WordIndexKVStore()
		{
			std::cerr << "Destroyed WordIndexKVStore" << std::endl;
		}

        static const string wordToString(const string& field, const string& word){
	        return field + '/' + word;
        }

		int Put(const std::string& field, const std::string& token, unsigned int value)
		{
			return Put(wordToString(field,token), value);
		}

		int Get(const std::string& field, const std::string& token, unsigned int& value) const
		{
			return Get(wordToString(field, token), value);
		}
		
		int Put(const std::string& wordString, const unsigned int value)
		{
			cache.put(wordString,value);
			
			string v = ZUtil::getString(value);
			//TODO: this need to be batched too 
			if (store->Put(wordString, v).ok())
			{
				return 1;
			}
			
			return 0;
		}

		int Get(const string& wordString, unsigned int& value) const
		{	
			if (cache.get(wordString,value)){
				return 1;
			} 
					
			string v;
			if (store->Get(wordString, v).ok())
			{
				value = ZUtil::getUInt(v);
				
				return 1;
			}

			return 0;
		}		
};

#endif // WORDINDEX_H
