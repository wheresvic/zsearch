#ifndef NAMESPACEKVSTORE_H
#define NAMESPACEKVSTORE_H

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>

#include "IKVStore.h"
#include "ZUtil.hpp"
#include "ZException.hpp"


namespace KVStore
{

	class NameSpaceKVStore : public IKVStore
	{
		private:

			const char ns;
			std::shared_ptr<KVStore::IKVStore> store;

		public:

			NameSpaceKVStore(const char ns, std::shared_ptr<KVStore::IKVStore> store) :  ns(ns), store(store)
			{

			}

			~NameSpaceKVStore()
			{
				std::cerr << "Destroyed NameSpaceKVStore " << ns << std::endl;
			}

			Status Open()
			{
				return Status::OK();
				// return store->Open();
			}

			Status Put(const std::string& key, const std::string& value)
			{
				string nkey = key;
				nkey.insert(0, 1, ns);
				return store->Put(nkey, value);
			}

			Status Put(uint64_t key, const std::string& value)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				keystr.insert(0, 1, ns);
				return store->Put(keystr, value);
			}

			Status Get(const std::string& key, std::string* value)
			{
				string nkey = key;
				nkey.insert(0, 1, ns);
				return store->Get(nkey, value);
			}

			Status Get(const std::string& key, std::string& value)
			{
				string nkey = key;
				nkey.insert(0, 1, ns);
				return store->Get(nkey, value);
			}

			Status Get(uint64_t key, std::string& value)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				keystr.insert(0, 1, ns);
				return store->Get(keystr, value);
			}

			Status Delete(uint64_t key)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				keystr.insert(0, 1, ns);
				return store->Delete(keystr);
			}

			Status Delete(const std::string& key)
			{
				string nkey = key;
				nkey.insert(0, 1, ns);
				return store->Delete(nkey);
			}

			void Compact()
			{
				store->Compact();
			}
			/*

			Status Put(const std::vector<std::pair<unsigned int, std::string>>& writes)
			{
				std::vector<std::pair<std::string, std::string>> writesNs;

				for (auto iter : writes)
				{
					std::string keystr;
					ZUtil::PutVarint64(keystr, iter.first);
					keystr.insert(keystr.begin(), ns);
					writesNs.push_back(pair<std::string, std::string>(keystr, iter.second));
				}

				return store->Put(writesNs);
			}

			Status Put(const std::vector<std::pair<std::string, std::string>>& writes)
			{
				std::vector<std::pair<std::string, std::string>> writesNs;

				for (auto iter : writes)
				{
					std::string keystr = iter.first;
					writesNs.push_back(pair<std::string, std::string>(keystr, iter.second));
				}

				return store->Put(writesNs);
			}

            unsigned int GetBatchSize(){
               return store->GetBatchSize();
            }
			
            void PutBatch(const std::string& key, const std::string& value){
				string keystr = key;
				keystr.insert(0, 1, ns);
				store->PutBatch(keystr, value);
            }

			void PutBatch(uint64_t key, const std::string& value){
				string keystr;
			    ZUtil::PutVarint64(keystr, key);
			    keystr.insert(0, 1, ns);
				store->PutBatch(keystr,value);
			}
			
			void DeleteBatch(const std::string& key){
				string keystr = key;
				keystr.insert(0, 1, ns);
				store->DeleteBatch(keystr);
			}
			
			void ClearBatch(){
				store->ClearBatch();
			}
			
			Status writeBatch(){
				return store->writeBatch();
			}*/
	}; // end class


} // namespace KVStore

#endif
