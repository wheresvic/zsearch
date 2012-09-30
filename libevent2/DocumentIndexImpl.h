
#ifndef DOCUMENTINDEXIMPL_H
#define DOCUMENTINDEXIMPL_H

#include "IDocumentIndex.h"
#include <map>
#include <memory>

using namespace std;

class DocumentIndexImpl : public IDocumentIndex
{
	public:

		void addDoc(unsigned int docId, shared_ptr<IDocument> doc);

		void removeDoc(unsigned int docId);

		const map<unsigned int, shared_ptr<IDocument>>& getDocuments();

	private:
		map<unsigned int, shared_ptr<IDocument>> documents;
};

#endif
