/**
 * Implementation of the union set of multiple DocIdSets (which essentially is a merged set of thes DocIdSets). 
 */
#ifndef LAZY_OR_SET_H__
#define LAZY_OR_SET_H__

#include "CompressedSet.h"

class LazyOrSetIterator : public Set::Iterator {
	private:
		class Item {
		  public:
			shared_ptr<Set::Iterator> iter;
			unsigned int doc;
			Item(shared_ptr<Set::Iterator> it){
				iter = it;
				doc = 0;
			}	
		};
		unsigned _curDoc;
		vector<shared_ptr<Item>> _heap;
		int _size;
		void heapRemoveRoot();
		void heapAdjust();
	public:
		LazyOrSetIterator(vector<shared_ptr<Set>> sets);
		unsigned int docID();
		unsigned int nextDoc();
		unsigned int Advance(unsigned int target);
};

class LazyOrSet : public Set 
{
	private:
		const int INVALID = -1;
		vector<shared_ptr<Set>> sets;
		int _size = INVALID;

	public:
	
		LazyOrSet(vector<shared_ptr<Set>> docSets);
		
		shared_ptr<Set::Iterator>  iterator()  const;
	
		//Override
		int size();
		
		bool find(unsigned int val) const;
};
	

#endif  //LAZY_OR_SET_H__