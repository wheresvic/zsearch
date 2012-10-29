/**
 * Implementation of the union set of multiple DocIdSets (which essentially is a merged set of thes DocIdSets). 
 */
#ifndef LAZY_OR_SET_H__
#define LAZY_OR_SET_H__
#include "Set.h"
#include <vector>
#include <memory>
class LazyOrSetIterator : public Set::Iterator {
	private:
		class Item {
		  public:
			shared_ptr<Set::Iterator> iter;
			int doc;
			Item(shared_ptr<Set::Iterator> it){
				iter = it;
				doc = -1;
			}	
		};
		int _curDoc;
		vector<shared_ptr<Item>> _heap;
		int _size;
		void heapRemoveRoot();
		void heapAdjust();
	public:
		LazyOrSetIterator(vector<shared_ptr<Set>> sets);
		int docID();
		int nextDoc();
		int Advance(int target);
};

class LazyOrSet : public Set {
	private:
		const int INVALID = -1;
		vector<shared_ptr<Set>> sets;
		int _size = INVALID;
	public:
		LazyOrSet(vector<shared_ptr<Set>> docSets){
			sets = docSets;
		}
		
		shared_ptr<Set::Iterator>  iterator()  const {
		   shared_ptr<Set::Iterator> it(new LazyOrSetIterator(sets));
	       return it;
	    }
	
		//Override
		int size() {
		    if(_size==INVALID){
		      _size=0;
		      LazyOrSetIterator it(sets);
		      while(it.nextDoc()!=NO_MORE_DOCS)
		          _size++;
		    }
		    return _size;
		}
		
		bool find(unsigned int val) const { 
			LazyOrSetIterator finder(sets);
			int docid = finder.Advance(val);
			return docid != NO_MORE_DOCS && docid == val;
		}
};
		
LazyOrSetIterator::LazyOrSetIterator(vector<shared_ptr<Set>> sets){
	_curDoc = -1;
	_size = sets.size();
	for (auto set : sets) {
		_heap.push_back(shared_ptr<Item>(new Item(set->iterator())));
	}
	if (_size == 0){
		_curDoc = NO_MORE_DOCS;
	}
}
	
int LazyOrSetIterator::docID() {
    return _curDoc;
}
	
int LazyOrSetIterator::nextDoc() {
    if(_curDoc == NO_MORE_DOCS) {
        return NO_MORE_DOCS;
    }

    shared_ptr<Item> top = _heap[0];
    while(true) {
      shared_ptr<Set::Iterator> topIter = top->iter;
      int docid;
      if((docid = topIter->nextDoc())!=NO_MORE_DOCS) {
        top->doc = docid;
        heapAdjust();
      } else {
        heapRemoveRoot();
        if(_size == 0) {
	        return (_curDoc = NO_MORE_DOCS);
        }
      }
      top = _heap[0];
      int topDoc = top->doc;
      if(topDoc > _curDoc) {
        return (_curDoc = topDoc);
      }
    }
}
	
int LazyOrSetIterator::Advance(int target) {
  if(_curDoc == NO_MORE_DOCS) {
    return NO_MORE_DOCS;
  }
  
  if(target <= _curDoc) {
    target = _curDoc + 1;
  }
  
  shared_ptr<Item> top = _heap[0];
  while(true) {
    shared_ptr<Set::Iterator> topIter = top->iter;
    int docid;
    if((docid = topIter->Advance(target))!=NO_MORE_DOCS) {
      top->doc = docid;
      heapAdjust();
    } else {
      heapRemoveRoot();
      if (_size == 0) {
        return (_curDoc = NO_MORE_DOCS);
      }
    }
    top = _heap[0];
    int topDoc = top->doc;
    if(topDoc >= target) {
      return (_curDoc = topDoc);
    }
  }
}

// Remove the root Scorer from subScorers and re-establish it as a heap
void LazyOrSetIterator::heapRemoveRoot(){
  _size--;
  if (_size > 0)
  {
    shared_ptr<Item> tmp = _heap[0];
    _heap[0] = _heap[_size];
    _heap[_size] = tmp; // keep the finished iterator at the end for debugging
    heapAdjust();
  }
}


/**
 * The subtree of subScorers at root is a min heap except possibly for its root element.
 * Bubble the root down as required to make the subtree a heap.
 */
void LazyOrSetIterator::heapAdjust(){
    shared_ptr<Item> top = _heap[0];
    int doc = top->doc;
    int size = _size;
    int i = 0;
    
    while(true)
    {
      int lchild = (i<<1)+1;
      if(lchild >= size) break;
      
      shared_ptr<Item> left = _heap[lchild];
      int ldoc = left->doc;
      
      int rchild = lchild+1;
      if(rchild < size){
        shared_ptr<Item> right = _heap[rchild];
        int rdoc = right->doc;
        
        if(rdoc <= ldoc)
        {
          if(doc <= rdoc) break;
          
          _heap[i] = right;
          i = rchild;
          continue;
        }
      }
      
      if(doc <= ldoc) break;
      
      _heap[i] = left;
      i = lchild;
    }
    _heap[i] = top;	 
}

#endif  //LAZY_OR_SET_H__