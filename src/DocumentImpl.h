
#ifndef DOCUMENTIMPL_H
#define DOCUMENTIMPL_H

#include <map>
#include <exception>
#include <string>
#include <memory>
#include "IDocument.h"

using namespace std;

class DocumentImpl : public IDocument
{
	public:

		DocumentImpl();

		DocumentImpl(const string& xml);

		void addEntry(const string& key, const string& value);
		
		void getEntry(const string& key, string& value);

		const map<string, string>& getEntries();
		
		void write(ostream & out);

	private:

		map<string, string> entries;

};

#endif
