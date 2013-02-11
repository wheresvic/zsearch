
#ifndef DOCUMENTSTORELEVELDB_H
#define DOCUMENTSTORELEVELDB_H

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <sstream>
#include "KVStoreLevelDb.h"
#include "ZException.hpp"

using namespace std;

class DocumentStoreLevelDb : public IDocumentStore
{
	private:

		std::shared_ptr<KVStore::IKVStore> store;

	public:

		DocumentStoreLevelDb(std::shared_ptr<KVStore::IKVStore> store) : store(store)
		{
			store->Open();
		}

		~DocumentStoreLevelDb()
		{
			std::cerr << "Destroyed DocumentStoreLevelDb" << std::endl;
		}

		void addDoc(unsigned int docId, const shared_ptr<IDocument>& doc)
		{
			stringstream ss;
			doc->write(ss);
			string d = ss.str();

			if (!(store->Put(docId, d).ok()))
			{
				throw ZException("Could not put document into LevelDb");
			}
		}

		void removeDoc(unsigned int docId)
		{
			store->Delete(docId);	// status could really only be Ok or NotFound
		}

		const map<unsigned int, shared_ptr<IDocument>>& getDocuments()
		{
			throw ZException("Not implemented - will be deprecated");
		}

		int Get(unsigned int docId, shared_ptr<IDocument>& doc) const
		{
			string d;

			if (store->Get(docId, d).ok())
			{
				doc->construct(d);

				/*
				cout << "got docId " << docId << endl << d << endl;

				try
				{
					doc->construct(d);
				}
				catch (const string& ex)
				{
					cerr << ex << endl;
				}
				catch (const exception& ex)
				{
					cerr << ex.what() << endl;
				}
				catch (...)
				{
					cerr << "wtf" << endl;
				}
				*/

				return 1;
			}

			return 0;
		}

};

#endif
