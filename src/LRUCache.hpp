#include <iostream>
#include <vector>
#include <unordered_map>

using namespace std;


template<class K, class T>
struct LRUCacheEntry
{
	K key;
	T data;
	LRUCacheEntry* prev;
	LRUCacheEntry* next;
};

template<class K, class T>
class LRUCache
{
private:
    mutable unordered_map<K, LRUCacheEntry<K,T>* >	_mapping;
	vector< LRUCacheEntry<K,T>* >		_freeEntries;
	mutable LRUCacheEntry<K,T> *			head;
	mutable LRUCacheEntry<K,T> *			tail;
	mutable LRUCacheEntry<K,T> *			entries;
public:
	LRUCache(size_t size){
		entries = new LRUCacheEntry<K,T>[size];
		for (int i=0; i<size; i++)
			_freeEntries.push_back(entries+i);
		head = new LRUCacheEntry<K,T>;
		tail = new LRUCacheEntry<K,T>;
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
	void put(const K key, const T data)
	{
		LRUCacheEntry<K,T>* node = _mapping[key];
		if(node)
		{
			// refresh the link list
			detach(node);
			node->data = data;
			attach(node);
		}
		else{
			if ( _freeEntries.empty() )
			{
				node = tail->prev;
				detach(node);
				_mapping.erase(node->key);
				node->data = data;
				node->key = key;
				attach(node);
			}
			else{
				node = _freeEntries.back();
				_freeEntries.pop_back();
				node->key = key;
				node->data = data;
				_mapping[key] = node;
				attach(node);
			}
		}
	}

	bool get(const K key,T& value) const
	{
		LRUCacheEntry<K,T>* node = _mapping[key];
		if(node)
		{
			detach(node);
			attach(node);
			value = node->data;
			return true;
		}
		else return false;
	}

private:
	void detach(LRUCacheEntry<K,T>* node) const
	{
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	void attach(LRUCacheEntry<K,T>* node) const
	{
		node->next = head->next;
		node->prev = head;
		head->next = node;
		node->next->prev = node;
	}
};
