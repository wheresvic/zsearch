
#include "DocumentStoreImpl.h"
#include <map>
#include <memory>

using namespace std;

void DocumentStoreImpl::addDoc(unsigned int docId, shared_ptr<IDocument> doc)
{
	documents.insert(make_pair(docId++, doc));
}

void DocumentStoreImpl::removeDoc(unsigned int docId)
{
	documents.erase(docId);
}

const map<unsigned int, shared_ptr<IDocument>>& DocumentStoreImpl::getDocuments()
{
	return documents;
}

