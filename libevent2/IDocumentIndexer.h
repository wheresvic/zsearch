
#ifndef IDOCUMENTINDEXER_H
#define IDOCUMENTINDEXER_H


#include "IDocument.h"
#include <string>
#include <memory>

using namespace std;

class IDocumentIndexer
{
	public:
		virtual ~IDocumentIndexer() { }

		virtual void addDoc(unsigned long, shared_ptr<IDocument> doc) = 0;
		virtual void removeDoc(unsigned long docId) = 0;

		// need to add iterator class instead of exposing the underlying implementation
		virtual const map<unsigned long, shared_ptr<IDocument>>& getDocuments() = 0;
};

#endif
