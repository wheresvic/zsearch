#include "LazyOrSet.h"


LazyOrSet::LazyOrSet(vector<shared_ptr<Set>> docSets)
{
	sets = docSets;
}

LazyOrSet::LazyOrSet(shared_ptr<Set>& left,shared_ptr<Set> & right)
{
    sets.push_back(left);
    sets.push_back(right);
}


shared_ptr<Set::Iterator>  LazyOrSet::iterator() const 
{
   shared_ptr<Set::Iterator> it(new LazyOrSetIterator(sets));
   return it;
}

unsigned int LazyOrSet::size()  const
{
	if(_size==INVALID){
	  _size=0;
	  LazyOrSetIterator it(sets);
	  while(it.nextDoc()!=NO_MORE_DOCS)
		  _size++;
	}
	return _size;
}

bool LazyOrSet::find(unsigned int val) const 
{ 
	LazyOrSetIterator finder(sets);
	unsigned int docid = finder.Advance(val);
	return docid != NO_MORE_DOCS && docid == val;
}

		
LazyOrSetIterator::LazyOrSetIterator(vector<shared_ptr<Set>> sets){
	_curDoc = 0;
	_size = sets.size();
	for (auto set : sets) {
		_heap.push_back(shared_ptr<Item>(new Item(set->iterator())));
	}
	if (_size == 0){
		_curDoc = NO_MORE_DOCS;
	}
}
	
unsigned int LazyOrSetIterator::docID() {
    return _curDoc;
}
	
unsigned int LazyOrSetIterator::nextDoc() {
    if(_curDoc == NO_MORE_DOCS) {
        return NO_MORE_DOCS;
    }

    shared_ptr<Item> top = _heap[0];
    while(true) {
      shared_ptr<Set::Iterator> topIter = top->iter;
      unsigned int docid;
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
      unsigned int topDoc = top->doc;
      if(topDoc > _curDoc) {
        return (_curDoc = topDoc);
      }
    }
}
	
unsigned int LazyOrSetIterator::Advance(unsigned int target) {
  if(_curDoc == NO_MORE_DOCS) {
    return NO_MORE_DOCS;
  }
  
  if(target <= _curDoc) {
    target = _curDoc + 1;
  }
  
  shared_ptr<Item> top = _heap[0];
  while(true) {
    shared_ptr<Set::Iterator> topIter = top->iter;
    unsigned int docid;
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
    unsigned int topDoc = top->doc;
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
