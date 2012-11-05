#ifndef INVERTED_INDEX_SIMPLE_H
#define INVERTED_INDEX_SIMPLE_H

#include "IInvertedIndex.hpp"
#include <memory>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include "../varint/Set.h"
#include "../varint/CompressedSet.h"
#include "KVStoreLevelDb.hpp"

class InvertedIndexSimple : public IInvertedIndex
{
private:
	
	KVStore::KVStoreLevelDb store;	
	
	void printSet(unsigned int wordId, CompressedSet& set)
	{
		std::cout << "wordId: [" << wordId << "]" << std::endl;
		shared_ptr<Set::Iterator> it = set.iterator();
		std::cout << "docid: [" ;
		
		while(it->nextDoc()!= NO_MORE_DOCS) 
		{
			std::cout << it->docID()  << ", ";		
		}
		
		std::cout << "]" << std::endl;
	}
		
public:
	
	InvertedIndexSimple() : store("/tmp/InvertedIndex")
	{
		store.Open();
	}

	int get(unsigned int wordId, CompressedSet*& set) 
	{
		string bitmap;
		if(store.Get(wordId,bitmap).ok())
		{
			stringstream bitmapStream(bitmap);
			set = new CompressedSet();
			set->read(bitmapStream);
			return 1;
		} 
		
		return 0;		
	}

	bool exist(unsigned int wordId)
	{
		string ret;
		bool found = store.Get(wordId,ret).ok();
		return found;
	}


	int put(unsigned int wordId, CompressedSet& set) 
	{
		stringstream ss;
		set.write(ss);
		string bitmap = ss.str();

		if (store.Put(wordId,bitmap).ok())
		{
			return 1;
		}
		
		return 0;	
	}

	int add(unsigned int wordId, unsigned int docid)
	{		
		if (exist(wordId))
		{
			CompressedSet *set;
			get(wordId,set);
			set->addDoc(docid);
			put(wordId,*set);
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
};

#endif