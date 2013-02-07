#ifndef WORDINDEXKVSTORE_H
#define WORDINDEXKVSTORE_H

#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include "KVStoreLevelDb.h"
#include "ZException.hpp"
#include "Word.hpp"
#include "ZUtil.hpp"

using namespace std;

class WordIndexKVStore
{
	private:
		
		std::shared_ptr<KVStore::IKVStore> store;

	public:

		WordIndexKVStore(std::shared_ptr<KVStore::IKVStore> store) : store(store)
		{
			// store->Open();
		}
		
		~WordIndexKVStore()
		{
			std::cerr << "Destroyed WordIndexKVStore" << std::endl;
		}

		int Put(const std::string& field, const std::string& token, unsigned int value)
		{
			Word word(field, token);
			return Put(word, value);
		}

		int Get(const std::string& field, const std::string& token, unsigned int& value) const
		{
			Word word(field, token);
			return Get(word, value);
		}
		
		int Put(const Word& word, unsigned int value)
		{
			string v = ZUtil::getString(value);
			
			if (store->Put(word.toString(), v).ok())
			{
				return 1;
			}
			
			return 0;
		}

		int Get(const Word& word, unsigned int& value) const
		{
			string v;
		
			if (store->Get(word.toString(), v).ok())
			{
				value = ZUtil::getUInt(v);
				return 1;
			}

			return 0;
		}		
};

#endif // WORDINDEX_H
