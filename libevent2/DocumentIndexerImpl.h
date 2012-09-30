
#ifndef DOCUMENTINDEXERIMPL_H
#define DOCUMENTINDEXERIMPL_H

#include "IDocumentIndexer.h"
#include <map>
#include <memory>

using namespace std;

class DocumentIndexerImpl : public IDocumentIndexer
{
	public:

		void addDoc(unsigned long docId, shared_ptr<IDocument> doc);

		void removeDoc(unsigned long docId);

		const map<unsigned long, shared_ptr<IDocument>>& getDocuments();

	private:
		map<unsigned long, shared_ptr<IDocument>> documents;
};

#endif
