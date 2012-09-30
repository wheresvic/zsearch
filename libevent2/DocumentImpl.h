
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

		void addEntry(const string& key, const string& value);

		const map<string, string>& getEntries();

	private:

		map<string, string> entries;

};

#endif
