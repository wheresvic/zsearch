
#include "DocumentIndexImpl.h"
#include <map>
#include <memory>

using namespace std;

void DocumentIndexImpl::addDoc(unsigned int docId, shared_ptr<IDocument> doc)
{
	documents.insert(make_pair(docId++, doc));
}

void DocumentIndexImpl::removeDoc(unsigned int docId)
{
	documents.erase(docId);
}

const map<unsigned int, shared_ptr<IDocument>>& DocumentIndexImpl::getDocuments()
{
	return documents;
}

