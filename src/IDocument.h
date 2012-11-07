

#ifndef IDOCUMENT_H
#define IDOCUMENT_H


#include <map>
#include <exception>
#include <string>
#include <memory>
#include <iostream>

using namespace std;

class IDocument
{
	public:

		virtual void addEntry(const string& key, const string& value) = 0;

		virtual const map<string, string>& getEntries() = 0;
		
		virtual void getEntry(const string& key, string& value) = 0;
		
		virtual void write(ostream & out) = 0;

		virtual ~IDocument() { }
};

#endif
