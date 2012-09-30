
#ifndef IDOCUMENTINDEX_H
#define IDOCUMENTINDEX_H


#include "IDocument.h"
#include <string>
#include <memory>

using namespace std;

class IDocumentIndex
{
	public:
		virtual ~IDocumentIndex() { }

		virtual void addDoc(unsigned int, shared_ptr<IDocument> doc) = 0;
		virtual void removeDoc(unsigned int docId) = 0;

		// need to add iterator class instead of exposing the underlying implementation
		virtual const map<unsigned int, shared_ptr<IDocument>>& getDocuments() = 0;
};

#endif
