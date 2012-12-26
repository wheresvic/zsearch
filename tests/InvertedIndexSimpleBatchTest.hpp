
#include "src/InvertedIndexSimpleBatch.hpp"
#include "lib/tpunit++.hpp"
#include "src/KVStoreInMemory.h"
#include "varint/SetFactory.h"
#include "varint/BasicSetFactory.h"
#include <memory>

using namespace std;

/**
 * Test InvertedIndex
 */
struct InvertedIndexSimpleBatchTest : tpunit::TestFixture
{
	InvertedIndexSimpleBatchTest() : tpunit::TestFixture
	(
		TEST(InvertedIndexSimpleBatchTest::testBasicSet),
		TEST(InvertedIndexSimpleBatchTest::testCompressedSet)
	)
	{ }

	void testBasicSet()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexBasicSet");
		InvertedIndexSimpleBatch invertedIndex(invertedIndexStore, setFactory);

		invertedIndex.add(1, 44);
		invertedIndex.add(1, 46);
		invertedIndex.add(1, 2);
		invertedIndex.add(1, 45);
		invertedIndex.add(1, 7);

		ASSERT_TRUE(invertedIndex.exist(1));
	}

	void testCompressedSet()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<SetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexCompressedSet");
		InvertedIndexSimpleBatch invertedIndex(invertedIndexStore, setFactory);
		invertedIndex.add(1, 44);
		ASSERT_TRUE(invertedIndex.exist(1));
	}

};
