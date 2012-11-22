
#include "src/InvertedIndexImpl.hpp"
#include "lib/tpunit++.hpp"
#include "src/KVStoreInMemory.h"
#include "varint/SetFactory.h"
#include "varint/BasicSetFactory.h"
#include <memory>

using namespace std;

/**
 * Test InvertedIndex
 */
struct InvertedIndexImplTest : tpunit::TestFixture
{
	InvertedIndexImplTest() : tpunit::TestFixture
	(
		TEST(InvertedIndexImplTest::testBasicSet),
		TEST(InvertedIndexImplTest::testCompressedSet)
	)
	{ }

	void testBasicSet()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexBasicSet");
		InvertedIndexImpl invertedIndex(invertedIndexStore, setFactory);
		invertedIndex.add(1, 44);
		ASSERT_TRUE(invertedIndex.exist(1));
	}

	void testCompressedSet()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<SetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexCompressedSet");
		InvertedIndexImpl invertedIndex(invertedIndexStore, setFactory);
		invertedIndex.add(1, 44);
		ASSERT_TRUE(invertedIndex.exist(1));
	}
	
};
