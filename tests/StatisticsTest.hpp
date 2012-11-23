
#include "src/Statistics.hpp"
#include "lib/tpunit++.hpp"
#include <memory>
#include <iostream>
#include <chrono>
#include <unordered_map>

using namespace std;

/**
 * Test Statistics
 */
struct StatisticsTest : tpunit::TestFixture
{
	StatisticsTest() : tpunit::TestFixture
	(
		TEST(StatisticsTest::testQueries),
		TEST(StatisticsTest::testRequestTimes)
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

	void testRequestTimes()
	{
		chrono::high_resolution_clock::time_point t0 = chrono::high_resolution_clock::now();
		Statistics s;
		chrono::high_resolution_clock::time_point t1 = chrono::high_resolution_clock::now();
		
		chrono::nanoseconds timeTaken = chrono::duration_cast<chrono::nanoseconds>(t1 - t0);
		
		string req = "creating statistics";
		s.logRequestTime(req, timeTaken);
		
		auto requestTimes = s.getRequestTimes(req);
		cout << req << " took ";
		for (auto ms : requestTimes)
		{
			cout << ms.count() << "ns ";
		}
		cout << endl;
	}
	
};
