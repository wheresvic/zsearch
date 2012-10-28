#ifndef IINVERTED_INDEX_H
#define IINVERTED_INDEX_H
#include "Word.hpp"
class IInvertedIndex
{
	public:
		virtual int  get(Word word, CompressedSet& set) = 0;
		virtual void put(Word word,CompressedSet& set) = 0;
		// Virtual destructor
		virtual ~IInvertedIndex() { }
};
#endif
