
#include "src/InvertedIndexImpl.hpp"
#include "lib/tpunit++.hpp"
#include "src/KVStoreInMemory.h"
#include "varint/SetFactory.h"
#include <memory>

using namespace std;

/**
 * Test InvertedIndex
 */
struct InvertedIndexImplTest : tpunit::TestFixture
{
	InvertedIndexImplTest() : tpunit::TestFixture
	(
		TEST(InvertedIndexImplTest::testBasic)
	)
	{ }

	void testBasic()
	{
		shared_ptr<SetFactory> setFactory = make_shared<SetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndex");
		InvertedIndexImpl invertedIndex(invertedIndexStore,setFactory);
		invertedIndex.add(1, 44);
		ASSERT_TRUE(invertedIndex.exist(1));
	}

};
