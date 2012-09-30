
#include "DocumentIndexerImpl.h"
#include <map>
#include <memory>

using namespace std;

void DocumentIndexerImpl::addDoc(unsigned long docId, shared_ptr<IDocument> doc)
{
	documents.insert(make_pair(docId++, doc));
}

void DocumentIndexerImpl::removeDoc(unsigned long docId)
{
	documents.erase(docId);
}

const map<unsigned long, shared_ptr<IDocument>>& DocumentIndexerImpl::getDocuments()
{
	return documents;
}
