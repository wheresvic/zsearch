#ifndef INVERTED_INDEX_IMPL_H
#define INVERTED_INDEX_IMPL_H

#include "IInvertedIndex.h"
#include <memory>
#include "varint/CompressedSet.h"
#include "IKVStore.h"

class InvertedIndexImpl : public IInvertedIndex
{
	private:

		std::shared_ptr<KVStore::IKVStore> store;
		void printSet(unsigned int wordId, CompressedSet& set);
	
	public:

		InvertedIndexImpl(std::shared_ptr<KVStore::IKVStore> store);

		int get(unsigned int wordId, shared_ptr<CompressedSet>& outset);
		
		bool exist(unsigned int wordId);
		
		int put(unsigned int wordId, CompressedSet& set);
		
		int add(unsigned int wordId, unsigned int docid);
	
};

#endif
