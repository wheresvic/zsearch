
#ifndef STATISTICS_HPP
#define STATISTICS_HPP


#include <string>
#include <map>
#include <set>
#include <vector>
#include <unordered_map>
#include <chrono>

using namespace std;

struct classcomp 
{
	bool operator() (const unsigned int& lhs, const unsigned int& rhs) const
	{
		return lhs > rhs;
	}
};

/**
 * TODO: Make thread-safe, consider singleton (will need to split into cpp and enforce correct linking order)
 * 
 */
class Statistics
{

	public: 
		
		/*
		static Statistics& getInstance()
		{
			static Statistics instance;
			return instance;
		}
		*/
		
		void logRequestTime(const string& request, const chrono::nanoseconds& timeTaken)
		{
			auto it = requestTimes.find(request);
			
			if (it != requestTimes.end())
			{
				(it->second).push_back(timeTaken);
			}
			else
			{
				vector<chrono::nanoseconds> times;
				times.push_back(timeTaken);
				requestTimes.insert(make_pair(request, times));
			}
		}
		
		vector<chrono::nanoseconds> getRequestTimes(const string& request)
		{
			vector<chrono::nanoseconds> times;
			
			auto it = requestTimes.find(request);
			
			if (it != requestTimes.end())
			{
				return it->second;
			}
			
			return times;
		}
		
		void addQuery(const string& query)
		{
			auto it = queries.find(query);
			
			if (it != queries.end())
			{
				++(it->second);
			}
			else
			{
				queries.insert(make_pair(query, 1));
			}
		}
		
		/**
		 * TODO: consider sorting queries manually in one pass
		 */
		map<unsigned int, set<string>, classcomp> getTopQueries(unsigned int count)
		{
			map<unsigned int, set<string>, classcomp> topQueries;
			
			for (auto it = queries.begin(); it != queries.end(); ++it)
			{
				auto found = topQueries.find(it->second);
				
				if (found != topQueries.end())
				{
					found->second.insert(it->first);
				}
				else
				{
					set<string> query_strings;
					query_strings.insert(it->first);
					topQueries.insert(make_pair(it->second, query_strings));
				}
			}
			
			if (topQueries.size() <= count)
			{
				return topQueries;
			}
			
			auto itq = topQueries.begin();
			for (unsigned int i = 0; i < count; ++i)
			{
				++itq;
			}
			
			map<unsigned int, set<string>, classcomp> topQueriesRet(topQueries.begin(), itq);
			return topQueriesRet;
			
		}
		
	private:
		
		/*
		Statistics();
		Statistics(Statistics const&);
		void operator=(Statistics const&);
		*/
		
		unordered_map<string, unsigned int> queries;
		unordered_map<string, vector<chrono::nanoseconds>> requestTimes;
		
};

#endif