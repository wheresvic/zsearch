#ifndef INVERTED_INDEX_SIMPLE_H
#define INVERTED_INDEX_SIMPLE_H

#include "IInvertedIndex.hpp"
#include <map>
#include <sstream>
#include <string>
#include "../varint/CompressedSet.h"

class InvertedIndexSimple : public IInvertedIndex
{
	public:
		 int get(unsigned int wordId, CompressedSet*& set) {
			set = new CompressedSet();
			auto found = invertedIndex.find(wordId);
			if (found != invertedIndex.end()){
			   string bitmap = found->second;
			   stringstream bitmapStream(bitmap);
			   set->read(bitmapStream);
			   return 1;
			} else {
			   return 0;
			}	
		 }
		
		 bool exist(unsigned int wordId){
			auto found = invertedIndex.find(wordId);
			return found != invertedIndex.end();
		 };
		
		
		 int put(unsigned int wordId, CompressedSet& set) {
			stringstream ss;
			set.write(ss);
			string bitmap = ss.str();
			invertedIndex.insert(make_pair(wordId,bitmap));
			return 1;
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
		


	private:
		// inverted index that maps words(wordId) to documents that contain it
		map<unsigned int, string> invertedIndex;
};

#endif