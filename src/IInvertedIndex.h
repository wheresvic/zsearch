#ifndef IINVERTED_INDEX_H
#define IINVERTED_INDEX_H
#include "varint/CompressedSet.h"

class IInvertedIndex
{
	public:
		virtual int get(unsigned int wordId, shared_ptr<CompressedSet>& outset) = 0;
		virtual int add(unsigned int wordId, unsigned int docid) = 0;
		
		// Virtual destructor
		virtual ~IInvertedIndex() { }
};
#endif
