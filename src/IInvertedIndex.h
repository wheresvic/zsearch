#ifndef IINVERTED_INDEX_H
#define IINVERTED_INDEX_H
#include "varint/CompressedSet.h"

class IInvertedIndex
{
	public:
		virtual int get(unsigned int wordId, CompressedSet*& set) = 0;
		virtual int put(unsigned int, CompressedSet& set) = 0;
		
		// Virtual destructor
		virtual ~IInvertedIndex() { }
};
#endif
