
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>

#include "IKVStore.h"
#include "KVStoreInMemory.h"

namespace KVStore
{
			void KVStoreInMemory::ConvertUint64ToString(std::string& dst, uint64_t v)
			{
				std::stringstream ss;
				ss << v;
				dst = ss.str();
			}

			KVStoreInMemory::KVStoreInMemory(const std::string& path) : IKVStore(path)
			{ }

			Status KVStoreInMemory::Open()
			{
				return Status::OK();
			}

			Status KVStoreInMemory::Put(const std::string& key, const std::string& value)
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

			Status KVStoreInMemory::Put(uint64_t key, const std::string& value)
			{
				std::string strKey;
				ConvertUint64ToString(strKey, key);
				return Put(strKey, value);
			}

			Status KVStoreInMemory::Get(const std::string& key, std::string* value)
			{
				auto iter = store.find(key);

				if (iter != store.end())
				{
					value->replace(0, value->size(), iter->second);
					return Status::OK();
				}

				return Status::NotFound();
			}

			Status KVStoreInMemory::Get(const std::string& key, std::string& value)
			{
				return Get(key, &value);
			}

			Status KVStoreInMemory::Get(uint64_t key, std::string& value)
			{
				std::string strKey;
				ConvertUint64ToString(strKey, key);
				return Get(strKey, &value);
			}

			Status KVStoreInMemory::Delete(const std::string& key)
			{
				auto iter = store.find(key);

				if (iter != store.end())
				{
					store.erase(iter);
					return Status::OK();
				}

				return Status::NotFound();
			}

			void KVStoreInMemory::Compact()
			{
				// do nothing
			}

} // namespace KVStore

