#ifndef FIELDKVSTORE_H
#define FIELDKVSTORE_H

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <unordered_set>

#include "IKVStore.h"
#include "ZException.hpp"
#include "ZUtil.hpp"
#include "Constants.hpp"

using namespace std;

class FieldKVStore
{
	private:

		unordered_set<string> fields;
		shared_ptr<KVStore::IKVStore> store;

	public:

		FieldKVStore(shared_ptr<KVStore::IKVStore> store) : store(store)
		{
			cerr << "Populating fields from store" << endl;

			string strFields;

			if (store->Get(zsearch::FIELDS_KEY, strFields).ok())
			{
				istringstream iss(strFields);
				string field;

				while (iss >> field)
				{
					// cerr << field << " ";
					fields.insert(field);
				}

				cerr << endl;
			}

		}

		~FieldKVStore()
		{
			cerr << "Persisting fields to store" << endl;

			ostringstream oss;

			for (auto field : fields)
			{
				oss << field << " ";
			}

			string strFields = oss.str();

			if (!store->Put(zsearch::FIELDS_KEY, strFields).ok())
			{
				cerr << "Error persisting fields to storage, db might be corrupt or invalid at startup" << endl;
			}

			cerr << "Destroyed FieldKVStore" << endl;
		}

		void put(const string& field)
		{
			fields.insert(field);
		}

		const unordered_set<string>& getFields() const
		{
			return fields;
		}

};

#endif
