#ifndef WORDINDEXKVSTORE_H
#define WORDINDEXKVSTORE_H

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <stdlib.h>  // rand
#include "IKVStore.h"
#include "ZException.hpp"
#include "ZUtil.hpp"
#include "LRUCache.hpp"

#include "bloom_filter.hpp"
using namespace std;

class WordIndexKVStore
{
	private:
		mutable LRUCache<string,unsigned int> cache;		
		std::shared_ptr<KVStore::IKVStore> store;
		bloom_filter* filter;
	public:

		WordIndexKVStore(std::shared_ptr<KVStore::IKVStore> store) : cache(656538),store(store)
		{
			bloom_parameters parameters;
			parameters.projected_element_count  = 1053691;
			parameters.false_positive_probability = 0.03;
			parameters.random_seed = 0xA5A5A5A55A5A5A5AULL;
			parameters.compute_optimal_parameters();
			filter = new bloom_filter(parameters);
		}
		
		~WordIndexKVStore()
		{
			delete filter;
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
			filter->insert(wordString);
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
			if (filter->contains(wordString)){ // might be in cache
				if (cache.get(wordString,value)){ 
					return 1; // found in cache
				}
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
