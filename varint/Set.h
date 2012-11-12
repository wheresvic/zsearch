#ifndef ABSTRACT_SET_H__
#define ABSTRACT_SET_H__

#include <memory>
#include <limits>

const int NO_MORE_DOCS = std::numeric_limits<int>::max();

class Set 
{
public:
	class Iterator {
		public:
		virtual int docID() = 0;

		virtual int nextDoc() = 0;
		// very efficient Advance method implemented with Skip list 
		virtual int Advance(int target) = 0;
		virtual ~Iterator() {};
	};
	virtual std::shared_ptr<Iterator> iterator()  const = 0;
	virtual void addDocs(unsigned int docids[],size_t start,size_t len) {};
	virtual void addDoc(unsigned int docId) {};
	// Free up unused memory in dynamic collection
	virtual void compact() {};
	// Force all docId to be part of a compressed chunk. 
	virtual void flush() {};
	virtual bool find(unsigned int target) const = 0;
	virtual ~Set() {}
};
#endif  // ABSTRACT_SET_H__