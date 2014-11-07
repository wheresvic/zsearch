#ifndef LRUCACHE_H__
#define LRUCACHE_H__

#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;

template<class T>
class ValueWrapper
{
public:
  bool dirty;
  T value;
  ValueWrapper() : dirty(false){}
  ValueWrapper(T input) : value(input), dirty(false){}
};

template<class K, class T>
struct LRUCacheEntry
{
	K key;
	T data;
	LRUCacheEntry* prev;
	LRUCacheEntry* next;
};


template<class K, class T, class F>
class LRUCache
{
private:
    mutable unordered_map<K, LRUCacheEntry<K,ValueWrapper<T>>* >	_mapping;
	vector< LRUCacheEntry<K,ValueWrapper<T>>*>		_freeEntries;
	mutable LRUCacheEntry<K,ValueWrapper<T>>*			head;
	mutable LRUCacheEntry<K,ValueWrapper<T>>*			tail;
	mutable LRUCacheEntry<K,ValueWrapper<T>>*			entries;
	F callback;
public:
	LRUCache(size_t size,F callbackp) : callback(callbackp) {

		entries = new LRUCacheEntry<K,ValueWrapper<T>>[size];
		for (int i=0; i<size; i++)
			_freeEntries.push_back(entries+i);
		head = new LRUCacheEntry<K,ValueWrapper<T>>;
		tail = new LRUCacheEntry<K,ValueWrapper<T>>;
		head->prev = NULL;
		head->next = tail;
		tail->next = NULL;
		tail->prev = head;
	}

	~LRUCache()
	{
		delete head;
		delete tail;
		delete [] entries;
	}
    void setCallback(F f){
    	callback = f;
    }

    void putOld(const K key, const T data,bool asDirty = false){
    		if ( _freeEntries.empty() )
			{
				LRUCacheEntry<K,ValueWrapper<T>>* node = tail->prev;
				detach(node);
				_mapping.erase(node->key);
				if (node->data.dirty)
				{
				    evict(node->key,node->data.value);
			    }
				node->data.value = data;
				node->data.dirty = asDirty;
				node->key = key;
				_mapping[key] = node;
				attach(node);
			}
			else{
				LRUCacheEntry<K,ValueWrapper<T>>* node = _freeEntries.back();
				_freeEntries.pop_back();
				node->key = key;
				node->data.value = data;
				node->data.dirty = asDirty;
				_mapping[key] = node;
				attach(node);
			}
    }

	void put(const K key, const T data,bool asDirty = true)
	{
		LRUCacheEntry<K,ValueWrapper<T>>* node = _mapping[key];
		if(node)
		{
			// refresh the link list
			detach(node);
			node->data.value = data;
			node->data.dirty = asDirty;
			attach(node);
		}
		else
		{
           putOld(key,data,asDirty);
		}
	}

	// TODO: make this a callback
	void evict(K key, T data){
       callback.evict(key,data);
	}

	bool get(const K key,T& value) const
	{
		LRUCacheEntry<K,ValueWrapper<T>>* node = _mapping[key];
		if(node)
		{
			detach(node);
			attach(node);
			value = node->data.value;
			return true;
		}
		else return false;
	}

	void evictAll(){
		LRUCacheEntry<K,ValueWrapper<T>>* node = tail->prev;
		while (node != head){
			if (node->data.dirty)
			{
                evict(node->key,node->data.value);
            }
            node = node->prev;
		}
	}

private:
	void detach(LRUCacheEntry<K,ValueWrapper<T>>* node) const
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	void attach(LRUCacheEntry<K,ValueWrapper<T>>* node) const
	{
		node->next = head->next;
		node->prev = head;
		head->next = node;
		node->next->prev = node;
	}
};
#endif  // LRUCACHE_H__
