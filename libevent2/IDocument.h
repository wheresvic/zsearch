

#ifndef IDOCUMENT_H
#define IDOCUMENT_H


#include <map>
#include <exception>
#include <string>
#include <memory>

using namespace std;

class IDocument
{
	public:

		virtual const string& getTitle() = 0;

		virtual void setTitle(const string& title) = 0;

		virtual void addEntry(const string& key, const string& value) = 0;

		virtual const map<string, string>& getEntries() = 0;

		// http://stackoverflow.com/questions/318064/how-do-you-declare-an-interface-in-c
		virtual ~IDocument() { }
};

#endif
