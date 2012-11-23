
#ifndef STATISTICS_HPP
#define STATISTICS_HPP


#include <string>
#include <map>
#include <set>
#include <unordered_map>

using namespace std;

struct classcomp 
{
	bool operator() (const unsigned int& lhs, const unsigned int& rhs) const
	{
		return lhs > rhs;
	}
};

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
		
};

#endif