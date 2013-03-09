

#ifndef IDOCUMENT_H
#define IDOCUMENT_H


#include <map>
#include <exception>
#include <string>
#include <memory>
#include <iostream>
#include <iterator>
#include <vector>
using namespace std;

class IDocument
{
	public:

		virtual void construct(const string& xml) = 0;
		
		virtual void write(ostream& out) = 0;
		
		virtual void readMini(const string& src) = 0;
		
		virtual void writeMini(ostream& out) = 0;
	
		virtual void addEntry(const string& key, const string& value) = 0;

		typedef std::vector<pair<string,string>>::const_iterator const_iterator;
		virtual const_iterator  begin() const = 0;
		virtual const_iterator  end()   const = 0;
		
		virtual void getEntry(const string& key, string& value) = 0;
		
		virtual ~IDocument() { }
};

#endif
