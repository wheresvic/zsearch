
#include "src/InvertedIndexSimpleBatch.hpp"
#include "lib/tpunit++.hpp"
#include "src/KVStoreInMemory.h"
#include "varint/SetFactory.h"
#include "varint/BasicSetFactory.h"
#include "varint/Set.h"
#include <memory>
#include <chrono>

using namespace std;

/**
 * Test InvertedIndex
 */
struct InvertedIndexSimpleBatchTest : tpunit::TestFixture
{
	InvertedIndexSimpleBatchTest() : tpunit::TestFixture
	(
		TEST(InvertedIndexSimpleBatchTest::testBasicSet1),
		TEST(InvertedIndexSimpleBatchTest::testBasicSet2),
		TEST(InvertedIndexSimpleBatchTest::testInvertedIndex),
		TEST(InvertedIndexSimpleBatchTest::testCompressedSet)
	)
	{ }

	void testBasicSet1()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexBasicSet");
		InvertedIndexSimpleBatch invertedIndex(invertedIndexStore, setFactory);

		invertedIndex.add(1, 44);
		invertedIndex.add(1, 46);
		invertedIndex.add(1, 2);
		invertedIndex.add(1, 45);
		invertedIndex.add(1, 7);
		invertedIndex.add(1, 8);

		std::this_thread::sleep_for(std::chrono::seconds(1));

		invertedIndex.add(1, 9);

		ASSERT_TRUE(invertedIndex.exist(1));
	}

	void testBasicSet2()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexBasicSet");
		InvertedIndexSimpleBatch invertedIndex(invertedIndexStore, setFactory);

		invertedIndex.shutDownBatchProcessor();

		invertedIndex.add(1, 44);
		invertedIndex.add(1, 46);
		invertedIndex.add(1, 2);
		invertedIndex.add(1, 45);
		invertedIndex.add(1, 7);
		invertedIndex.add(1, 8);

		std::this_thread::sleep_for(std::chrono::seconds(1));

		invertedIndex.add(1, 9);

		ASSERT_TRUE(invertedIndex.exist(1));
	}


	void testInvertedIndex()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<BasicSetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexBasicSet");
		InvertedIndexSimpleBatch invertedIndex(invertedIndexStore, setFactory);

		invertedIndex.shutDownBatchProcessor();
		invertedIndex.shutDownBatchProcessor();

		invertedIndex.add(1, 7);
		invertedIndex.add(1, 8);
		invertedIndex.add(1, 9);
		invertedIndex.add(1, 10);
		invertedIndex.add(1, 11);
		invertedIndex.add(1, 12);
		invertedIndex.add(1, 13);

		ASSERT_TRUE(invertedIndex.exist(1));

		invertedIndex.flush();

		shared_ptr<Set> set;
		invertedIndex.get(1, set);

		shared_ptr<Set::Iterator> it = set->iterator();

		unsigned int i = 7;

		while(it->nextDoc() != NO_MORE_DOCS)
		{
			cout << it->docID() << " ";
			ASSERT_TRUE(i == it->docID());
			++i;
		}

	}

	void testCompressedSet()
	{
		shared_ptr<ISetFactory> setFactory = make_shared<SetFactory>();
		shared_ptr<KVStore::IKVStore> invertedIndexStore = make_shared<KVStore::KVStoreInMemory>("/tmp/TestInvertedIndexCompressedSet");
		InvertedIndexSimpleBatch invertedIndex(invertedIndexStore, setFactory);

		invertedIndex.add(1, 44);
		ASSERT_FALSE(invertedIndex.exist(1));

		invertedIndex.flush();

		ASSERT_TRUE(invertedIndex.exist(1));
	}

};
