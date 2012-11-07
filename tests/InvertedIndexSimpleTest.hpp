
#include "../src/InvertedIndexSimple.hpp"
#include "../src/lib/tpunit++.hpp"

using namespace std;

/**
 * Test InvertedIndex
 */
struct InvertedIndexSimpleTest : tpunit::TestFixture
{
	InvertedIndexSimpleTest() : tpunit::TestFixture
	(
		TEST(InvertedIndexSimpleTest::testBasic)
	)
	{ }

	void testBasic()
	{
		InvertedIndexSimple invertedIndex;
		invertedIndex.add(1, 44);
		ASSERT_TRUE(invertedIndex.exist(1));
	}

};
