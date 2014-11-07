#ifndef ABSTRACT_SET_H__
#define ABSTRACT_SET_H__

#include <memory>
#include <limits>
#include <iostream>
const unsigned int NO_MORE_DOCS = std::numeric_limits<unsigned int>::max();

class Set
{
public:
	class Iterator {
		public:
		virtual unsigned int docID() = 0;
		//Calling nextDoc past end should have no effect
		virtual unsigned int nextDoc() = 0;
		// very efficient Advance method implemented with Skip list
		virtual unsigned int Advance(unsigned int target) = 0;
		virtual ~Iterator() {};
	};
	virtual std::shared_ptr<Iterator> iterator()  const = 0;

	virtual void addDocs(unsigned int* docids,size_t start,size_t len) { throw -101; };
	virtual void addDoc(unsigned int docId) { throw -102; };
	virtual void removeDocId(unsigned int docId) { throw -103; };
	// Free up unused memory in dynamic collection
	virtual void compact() { throw -104; };

	virtual bool find(unsigned int target) const = 0;

	virtual void write(std::ostream & out) { throw -105; };
	virtual void read(std::istream & in) { throw -106; };
	// virtual void size(std::istream & in) = 0;

	virtual unsigned int size() const = 0;

	virtual ~Set() {}

	bool equals(std::shared_ptr<Set> other){
        std::shared_ptr<Set::Iterator> it1 = this->iterator();
        std::shared_ptr<Set::Iterator> it2 = other->iterator();
        while( it1->nextDoc() != NO_MORE_DOCS ){
        	if (it2->nextDoc() == NO_MORE_DOCS){
        		return false;
        	}
            if (it1->docID() != it2->docID())
                return false;
        }
        return NO_MORE_DOCS == it2->nextDoc();
    }
};
#endif  // ABSTRACT_SET_H__
