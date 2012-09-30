
#include <list>
#include <string>

using namespace std;

class ISuggestor
{
	public:
		virtual ~ISuggestor() { }
		
		virtual list<string> complete(string partialQuery, string field) = 0;
};
