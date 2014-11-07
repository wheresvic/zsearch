#ifndef LAZY_AND_NOT_SET_H__
#define LAZY_AND_NOT_SET_H__
#include "Set.h"
#include "CompressedSet.h"

class LazyAndNotSet;

class LazyAndNotSetIterator : public Set::Iterator {
private:
	unsigned int _nextDelDoc;
    shared_ptr<Set::Iterator> _baseIter;
    shared_ptr<Set::Iterator> _notIter;
    int _currID;
public:
	LazyAndNotSetIterator(std::shared_ptr<Set::Iterator> baseIter, std::shared_ptr<Set::Iterator> notIter){
		_baseIter = baseIter;
		_notIter = notIter;
        _nextDelDoc = _notIter->nextDoc();
        _currID = -1;
	}

	unsigned int docID(){
		return _currID;
	}

	unsigned int nextDoc(){
	    _currID =_baseIter->nextDoc();
	    if (_nextDelDoc!=NO_MORE_DOCS){
	        while(_currID != NO_MORE_DOCS) {
	            if (_currID < _nextDelDoc) { 
	                return _currID;
	            } else {
	                if (_currID == _nextDelDoc) {
	                    _currID =_baseIter->nextDoc();
	                }
	                _nextDelDoc = _notIter->Advance(_currID);
	            }
	        }
	    }
	    return _currID;
	}

	unsigned int Advance(unsigned int target){
		_currID = _baseIter->Advance(target);
		if (_currID==NO_MORE_DOCS)
			return _currID;

		if (_nextDelDoc!=NO_MORE_DOCS) {
			_currID = _baseIter->docID();
			if (_currID<_nextDelDoc) return _currID;
			_nextDelDoc = _notIter->Advance(_currID);
			if (_currID==_nextDelDoc) return nextDoc();
		}
		return _currID;
	}
};

class LazyAndNotSet : public Set {
private:
	shared_ptr<Set> _baseSet;
	shared_ptr<Set> _notSet;
	mutable unsigned int setSize;
	mutable bool init = false;
public:	
	LazyAndNotSet(std::shared_ptr<Set> baseSet, std::shared_ptr<Set> notSet){
		setSize = 0;
		init = false;
		_baseSet =  baseSet;
		_notSet = notSet;
	}
	
	inline bool find(unsigned int val) const{
		LazyAndNotSetIterator finder(_baseSet->iterator(),_notSet->iterator());
	    unsigned docid = finder.Advance(val);
	    return docid != NO_MORE_DOCS && docid == val;
	}
	
	unsigned int size() const{
		// Do the size if we haven't done it so far.
		if(!init) {
		  LazyAndNotSetIterator dcit(_baseSet->iterator(),_notSet->iterator());
		  setSize = 0;
		  while(dcit.nextDoc() != NO_MORE_DOCS)
			 setSize++;
		}
		init = true;
		return setSize;
	}

    shared_ptr<Set::Iterator> iterator() const{
    	shared_ptr<Set::Iterator> it = make_shared<LazyAndNotSetIterator>(_baseSet->iterator(),_notSet->iterator());
		return it;
    }
	
};

#endif  // LAZY_AND_NOT_SET_H__
