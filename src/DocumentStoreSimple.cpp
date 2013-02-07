
#include "DocumentStoreSimple.h"
#include <iostream>
#include <map>
#include <memory>

using namespace std;

DocumentStoreSimple::DocumentStoreSimple() {
	// store.Open("/tmp/DocumentStore");	
}

DocumentStoreSimple::~DocumentStoreSimple()
{
	std::cerr << "Destroyed DocumentStoreSimple" << std::endl;
}

void DocumentStoreSimple::addDoc(unsigned int docId, const shared_ptr<IDocument>& doc) {
	//store.Put(docId,);
	documents.insert(make_pair(docId++, doc));
}

void DocumentStoreSimple::removeDoc(unsigned int docId) {
	documents.erase(docId);
}

const map<unsigned int, shared_ptr<IDocument>>& DocumentStoreSimple::getDocuments() {
	return documents;
}

int DocumentStoreSimple::Get(unsigned int docId, shared_ptr<IDocument>& doc) const {
	auto iter = documents.find(docId);

	if (iter != documents.end())
	{
		doc = iter->second;
		return 1;
	}
	return 0;
}