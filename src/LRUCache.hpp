#ifndef LRUCACHE_H
#define LRUCACHE_H

#include <list>
#include <unordered_map>
#include <assert.h>
#include "cedar.h"
using namespace std;

class LRUCache{
private:
        mutable list< pair<std::string,int> > item_list; 
        mutable cedar::da <int, -1, -2, false> trie;
        mutable unordered_map<std::string, decltype(item_list.begin()) > item_map;
        size_t cache_size;
private:
        void clean(void) {
	            if (item_map.size()>cache_size){
					std::cout << "cache full" << std::endl;
	            }
                while(item_map.size()>cache_size){
                        auto last_it = item_list.end(); last_it --;
                        item_map.erase(last_it->first);
                        item_list.pop_back();
                }
        };
public:
        LRUCache(int cache_size_):cache_size(cache_size_){
                
        };

        void put(const std::string &key, const int &val){
	            trie.update (key.c_str(), key.size(), val);
			    return;
        };

        bool get(const std::string &key,unsigned int& value) const  {
	            // add bloom filter her :)
			    const int val = trie.exactMatchSearch<int>(key.c_str(), key.size());
				value = val;
                return val!=-1;
        };
};
#endif