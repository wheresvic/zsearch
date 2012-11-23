
#include "src/Statistics.hpp"
#include "lib/tpunit++.hpp"
#include <memory>
#include <iostream>
#include <unordered_map>

using namespace std;

/**
 * Test Statistics
 */
struct StatisticsTest : tpunit::TestFixture
{
	StatisticsTest() : tpunit::TestFixture
	(
		TEST(StatisticsTest::testQueries)
	)
	{ }

	void printTopQueries(const map<unsigned int, set<string>, classcomp>& topQueries)
	{
		for (auto it = topQueries.begin(); it != topQueries.end(); ++it)
		{
			cout << it->first << " ";
			for (auto qs : it->second)
			{
				cout << qs << " ";
			}
			cout << endl;
		}
	}
	
	void testQueries()
	{
		Statistics s;
		
		s.addQuery("snoop");
		s.addQuery("dawg");
		s.addQuery("werd");
		s.addQuery("snoop");
		s.addQuery("snoop");
		s.addQuery("snoop");
		s.addQuery("dawg");
		s.addQuery("werd");
		s.addQuery("yo");
		
		auto topQueries = s.getTopQueries(3);
		printTopQueries(topQueries);
		
		ASSERT_EQUAL(topQueries.size(), 3);
		
		topQueries = s.getTopQueries(2);
		printTopQueries(topQueries);
		
		auto it = topQueries.begin();
		ASSERT_EQUAL(topQueries.size(), 2);
		
		ASSERT_EQUAL(it->first, 4);
		ASSERT_EQUAL((it->second).size(), 1);
		
		++it;
		ASSERT_EQUAL(it->first, 2);
		ASSERT_EQUAL((it->second).size(), 2);

	}

	
};
