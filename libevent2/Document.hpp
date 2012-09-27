#include <map>
#include <exception>
#include <string>

using namespace std;

class Document
{
	public:

		Document()
		{ }
		
		void addEntry(const string& key, const string& value)
		{
			entries.insert(make_pair(key, value));
		}
		
		const map<string, string>& getEntries()
		{
			return entries;
		}
		
	private:
	
		map<string, string> entries;

};

