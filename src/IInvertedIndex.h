#ifndef IINVERTED_INDEX_H
#define IINVERTED_INDEX_H

#include <memory>
#include "varint/Set.h"
class IInvertedIndex
{
	public:
		virtual int get(unsigned int wordId, std::shared_ptr<Set>& outset) = 0;
		virtual int add(unsigned int wordId, unsigned int docid) = 0;
		
		// Virtual destructor
		virtual ~IInvertedIndex() { }
};
#endif
