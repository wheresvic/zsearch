
#ifndef DOCUMENTIMPL_H
#define DOCUMENTIMPL_H

#include <map>
#include <exception>
#include <string>
#include "IDocument.h"

using namespace std;

class DocumentImpl : public IDocument
{
	public:

		DocumentImpl();

		DocumentImpl(const string& title);

		const string& getTitle();

		void addEntry(const string& key, const string& value);

		const map<string, string>& getEntries();

	private:

		string title;

		map<string, string> entries;

};

#endif
