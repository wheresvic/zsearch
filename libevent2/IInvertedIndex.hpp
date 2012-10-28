#ifndef IINVERTED_INDEX_H
#define IINVERTED_INDEX_H
#include "Word.hpp"
#include "../varint/CompressedSet.h"

class IInvertedIndex
{
	public:
		virtual int get(Word& word, CompressedSet& set) = 0;
		virtual int put(Word& word, CompressedSet& set) = 0;
		
		// Virtual destructor
		virtual ~IInvertedIndex() { }
};
#endif
