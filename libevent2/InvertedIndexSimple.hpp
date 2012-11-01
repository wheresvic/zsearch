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
#include "KVStore.hpp"
class InvertedIndexSimple : public IInvertedIndex
{
private:
	KVStore::KVStore store;
	
	char* EncodeVarint64(char* dst, uint64_t v) {
	  static const int B = 128;
	  unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	  while (v >= B) {
	    *(ptr++) = (v & (B-1)) | B;
	    v >>= 7;
	  }
	  *(ptr++) = static_cast<unsigned char>(v);
	  return reinterpret_cast<char*>(ptr);
	}

	void PutVarint64(std::string& dst, uint64_t v) {
	  char buf[10];
	  char* ptr = EncodeVarint64(buf, v);
	  dst.append(buf, ptr - buf);
	}
	
	
	void printSet(unsigned int wordId,CompressedSet& set){
		std::cout << "wordId: [" << wordId << "]" << std::endl;
		shared_ptr<Set::Iterator> it = set.iterator();
		std::cout << "docid: [" ;
		while(it->nextDoc()!= NO_MORE_DOCS) {
				std::cout << it->docID()  << ", ";
		
		}
	   std::cout << "]" << std::endl;
	}
		
public:
         InvertedIndexSimple(){
			store.Open("/tmp/InvertedIndex");
         }

		 int get(unsigned int wordId, CompressedSet*& set) {
			string key;
			PutVarint64(key,wordId);
			string bitmap;
			if(store.Get(key,bitmap).ok()){
				stringstream bitmapStream(bitmap);
				set = new CompressedSet();
			    set->read(bitmapStream);
			    return 1;
			} else {
				return 0;
			}
		 }
		
		 bool exist(unsigned int wordId){
			string key;
			PutVarint64(key,wordId);
			string ret;
			bool found = store.Get(key,ret).ok();

			return found;
		 };
		
		
		 int put(unsigned int wordId, CompressedSet& set) {
			string key;
			PutVarint64(key,wordId);
			
			stringstream ss;
			set.write(ss);
			string bitmap = ss.str();
			
			if (store.Put(key,bitmap).ok()){
				return 1;
			} else {
				return 0;
			}
		};
		
		int add(unsigned int wordId, unsigned int  docid){		
			if (exist(wordId)){
				CompressedSet *set;
				get(wordId,set);
				set->addDoc(docid);
				put(wordId,*set);
				delete set;
			} else {
				CompressedSet set;
				set.addDoc(docid);
				put(wordId,set);
			}
			return 1;
		};
};

#endif