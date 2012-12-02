#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <memory>
#include <exception>
#include <iostream>
#include <iterator>

#include "lib/tpunit++.hpp"
#include "TestUtils.hpp"
#include "src/SparseSet.hpp"
using namespace std;

struct SparseSetTest : tpunit::TestFixture
{
	SparseSetTest() : tpunit::TestFixture
	(
		TEST(SparseSetTest::testSet)
	)
	{}
	
	void testSet()
	{
		SparseSet set;
		
		set.insert(65535);
		set.insert(512);
		ASSERT_EQUAL(set.ismember(65534),false);
		ASSERT_EQUAL(set.ismember(65535),true);
		ASSERT_EQUAL(set.ismember(65536),false);
		
		const unsigned int* iter = set.begin();
		ASSERT_EQUAL(*iter,65535);
		iter++;
		ASSERT_EQUAL(*iter,512);
		iter++;
		ASSERT_EQUAL(iter,set.end());
	}
};

int main()
{
	SparseSetTest test;

	/**
	 * Run all of the registered tpunit++ tests. Returns 0 if
	 * all tests are successful, otherwise returns the number
	 * of failing assertions.
	 */
	return tpunit::Tests::Run();
}