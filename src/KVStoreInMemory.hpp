#ifndef KVSTORE_IN_MEMORY_H
#define KVSTORE_IN_MEMORY_H

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include <memory>

#include "IKVStore.h"
#include "ZUtil.hpp"

namespace KVStore
{

	class KVStoreInMemory : public IKVStore
	{
		private:

			std::map<std::string, std::string> store;
            std::vector<std::pair<std::string,std::string>> batch;
		public:

			KVStoreInMemory(const std::string& path) 
			{ }

			~KVStoreInMemory()
			{
			}

			Status Open()
			{
				return Status::OK();
			}

			Status Put(const std::string& key, const std::string& value)
			{
				auto iter = store.find(key);

				if (iter != store.end())
				{
					iter->second = value;
				}
				else
				{
					store.insert(std::make_pair(key, value));
				}

				return Status::OK();
			}

			Status Put(uint64_t key, const std::string& value)
			{
				std::string strKey;
				ZUtil::PutVarint64(strKey, key);
				return Put(strKey, value);
			}

			Status Get(const std::string& key, std::string* value)
			{
				auto iter = store.find(key);

				if (iter != store.end())
				{
					value->replace(0, value->size(), iter->second);
					return Status::OK();
				}

				return Status::NotFound();
			}

			Status Get(const std::string& key, std::string& value)
			{
				return Get(key, &value);
			}

			Status Get(uint64_t key, std::string& value)
			{
				std::string strKey;
				ZUtil::PutVarint64(strKey, key);
				return Get(strKey, &value);
			}

			Status Delete(uint64_t key)
			{
				std::string strKey;
				ZUtil::PutVarint64(strKey, key);
				return Delete(strKey);
			}
			
			Status Delete(const std::string& key)
			{
				auto iter = store.find(key);

				if (iter != store.end())
				{
					store.erase(iter);
					return Status::OK();
				}

				return Status::NotFound();
			}

			void Compact()
			{
				// do nothing
			}

			Status Put(const std::vector<std::pair<unsigned int, std::string>>& writes)
			{
				unsigned int countMissed = 0;

				for (auto write : writes)
				{
					if (!Put(write.first, write.second).ok())
					{
						++countMissed;
					}
				}

				if (countMissed)
				{
					return Status::Corruption();
				}

				return Status::OK();
			}

			Status Put(const std::vector<std::pair<std::string, std::string>>& writes)
			{
				unsigned int countMissed = 0;

				for (auto write : writes)
				{
					if (!Put(write.first, write.second).ok())
					{
						++countMissed;
					}
				}

				if (countMissed)
				{
					return Status::Corruption();
				}

				return Status::OK();
			}
			

			void PutBatch(const std::string& key, const std::string& value)
			{
				batch.push_back(std::pair<std::string,std::string>(key,value));
			}

			void PutBatch(uint64_t key, const std::string& value)
			{
				string keystr;
				ZUtil::PutVarint64(keystr, key);
				PutBatch(keystr,value);
			}

			void DeleteBatch(const std::string& key)
			{
				//batch.Delete(key);
			}

			void ClearBatch()
			{
				batch.clear();
			}

			Status writeBatch()
			{
				Put(batch);
			    return Status::OK();
			}
	}; // end class


} // namespace KVStore

#endif
