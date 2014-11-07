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
#include <cedarpp.h>
using namespace std;

class WordIndexKVStore
{
    struct EvictionCallback{
	  WordIndexKVStore* store;
	  EvictionCallback(WordIndexKVStore* storep): store(storep){

	  }
	  void evict(string key,unsigned int data){
	  	std::cout << "WordIndex" << std::endl;
		store->evictPut(key,data);
	  }  
    };
	private:
		mutable LRUCache<string,unsigned int,EvictionCallback> cache;		
		std::shared_ptr<KVStore::IKVStore> store;
		cedar::da <int, -1, -2, false> tree;
	public:

		WordIndexKVStore(std::shared_ptr<KVStore::IKVStore> store) : cache(16000,this),store(store)
		{
            load();
		}
		
		~WordIndexKVStore()
		{
			cache.evictAll();
			save();
			std::cerr << "Destroyed WordIndexKVStore" << std::endl;
		}

        void save(){
            tree.save("wordindex","wb",true);
        }

        void load(){

        	tree.open("wordindex");
        }
        void evictPut(string wordString,unsigned int value){
        	std::string v;
        	ZUtil::PutVarint32(&v,value);
        	store->Put(wordString, v);

        }

        static const string wordToString(const string& field, const string& word){
	        return field + '/' + word;
        }

		int Put(const std::string& field, const std::string& token, unsigned long long value)
		{
			return Put(wordToString(field,token), value);
		}

		int Get(const std::string& field, const std::string& token, unsigned long long& value) const
		{
			return Get(wordToString(field, token), value);
		}
		
		void cedarPut(const std::string&  key,const unsigned long long value){
            tree.update (key.c_str(), key.size()) = value;
		}

		bool cedarGet(const std::string& key,unsigned long long& value) const {
			int ret = tree.exactMatchSearch<int>(key.c_str(), key.size());
			if (ret >= 0){
                value = ret;
                return true;
			}
			return false;
		}

		int Put(const std::string& wordString, const unsigned long long value)
		{
		//	cache.put(wordString,value,true);
			cedarPut(wordString,value);
			return 1;
		}


		int Get(const string& wordString, unsigned long long& value) const
		{	
		//	if (cache.get(wordString,value)){ 
		//	    return 1; // found in cache
		//	}
			if (cedarGet(wordString,value)){ 
			    return 1; // found in cache
			}			
/*
			string v;
			if (store->Get(wordString, v).ok())
			{
				leveldb::Slice s(v);
			    ZUtil::GetVarint32(&s,&value);
				cache.putOld(wordString,value,false);
				return 1;
			} 
*/
			return 0;
		}		
};

#endif // WORDINDEX_H
