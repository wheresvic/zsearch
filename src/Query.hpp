
#include <string>
#include <set>
#include <list>
#include <map>

using namespace std;

/**
 * match document having field value between lowerTerm and upperTerm
 */
class QueryTermRange
{
	string field;
	string lowerTerm;
	string upperTerm;
};

/**
 * match document having field containing one of the terms
 */
class QueryTerm
{
	string field;
	set<string> terms;
};

class ResultSet
{
	string status;
	int matches;
	list<map<string, string>> docs;
};

class Searcher
{
	// start and length are for paging result
	ResultSet search(string query, int start, int length, list<QueryTermRange> queryRange, list<QueryTerm> queryTerm);
};

