#ifndef LAZY_AND_SET_H__
#define LAZY_AND_SET_H__
#include <vector>
#include "Set.h"
class LazyAndSet;

class LazyAndSetIterator : Set::Iterator {
private:
	int lastReturn; 
	vector<shared_ptr<Set::Iterator> > iterators;
	const LazyAndSet& set;
public:
	LazyAndSetIterator(const LazyAndSet* parent);
	int docID();
	int nextDoc();
	int Advance(int target);
};

class LazyAndSet : Set {
 public:
	vector<shared_ptr<Set> > sets_;
	int nonNullSize;
	int setSize;
	LazyAndSet(){
		sets_ = vector<shared_ptr<Set> >();
		nonNullSize = 0;
		setSize = -1;
	}
	
	LazyAndSet(vector<shared_ptr<Set> >& sets){
		sets_ = sets;
        nonNullSize = sets.size();
        setSize = -1;
	}
	
	inline bool find(unsigned int val) const { 
		LazyAndSetIterator finder(this);
		int docid = finder.Advance(val);
		return docid != NO_MORE_DOCS && docid == val;
	}
	
	int size() {
        // Do the size if we haven't done it so far.
        if(setSize < 0) {
          LazyAndSetIterator dcit(this);
          setSize = 0;
          while(dcit.nextDoc() != NO_MORE_DOCS)
             setSize++;
        }
        return setSize;
    }

	
	LazyAndSetIterator iterator() {
		LazyAndSetIterator it(this);
		return it;
	}
};

LazyAndSetIterator::LazyAndSetIterator(const LazyAndSet* parent) : set(*parent){
	lastReturn = -1;
	if (set.nonNullSize < 1)
	    throw string("Minimum one iterator required");
	
	for (vector<shared_ptr<Set> >::const_iterator it = set.sets_.begin(); it!=set.sets_.end(); it++){
		shared_ptr<Set> set  = *it;
		shared_ptr<Set::Iterator>  dcit = set->iterator();
		iterators.push_back(dcit);
	}
	lastReturn = (iterators.size() > 0 ? -1 : NO_MORE_DOCS);
}

int LazyAndSetIterator::docID() {
    return lastReturn;
}

int LazyAndSetIterator::nextDoc() {
    // DAAT
    if (lastReturn == NO_MORE_DOCS) 
       return NO_MORE_DOCS;    
    
    shared_ptr<Set::Iterator> dcit = iterators[0];
    int target = dcit->nextDoc();

    // shortcut: if it reaches the end of the shortest list, do not scan other lists
    if(target == NO_MORE_DOCS) { 
        return (lastReturn = target);
    }

    int size = iterators.size();
    int skip = 0;
    int i = 1;
   
    // i is ith iterator
    while (i < size) {
      if (i != skip) {
        dcit = iterators[i];
        int docId = dcit->Advance(target);
       
        // once we reach the end of one of the blocks, we return NO_MORE_DOCS
        if(docId == NO_MORE_DOCS) {
          return (lastReturn = docId);
        }
    
        if (docId > target) { //  cannot find the target in the next list
          target = docId;
          if(i != 0) {
            skip = i;
            i = 0;
            continue;
          } else { // for the first list, it must succeed as long as the docId is not NO_MORE_DOCS
            skip = 0;
          }
        }

      }
      i++;
    }
   
    return (lastReturn = target);
}

int LazyAndSetIterator::Advance(int target) {
     if (lastReturn == NO_MORE_DOCS) 
        return NO_MORE_DOCS;
    
      shared_ptr<Set::Iterator> dcit = iterators[0];
     target = dcit->Advance(target);
     if(target == NO_MORE_DOCS) { 
       return (lastReturn = target);
     }
     
     int size = iterators.size();
     int skip = 0;
     int i = 1;
     while (i < size) {
       if (i != skip) {
         dcit = iterators[i];
         int docId = dcit->Advance(target);
         if(docId == NO_MORE_DOCS) {
           return (lastReturn = docId);
         }
         if (docId > target) {
           target = docId;
           if(i != 0) {
             skip = i;
             i = 0;
             continue;
           } else {
             skip = 0;
           }
         }
       }
       i++;
     }
     return (lastReturn = target);
}
#endif  // LAZY_AND_SET_H__
