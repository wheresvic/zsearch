#ifndef KVSTORE_IN_MEMORY_H
#define KVSTORE_IN_MEMORY_H

#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "IKVStore.h"
#include <memory>

namespace KVStore
{

	class KVStoreInMemory : public IKVStore
	{
		private:

			void ConvertUint64ToString(std::string& dst, uint64_t v)
			{
				std::stringstream ss;
				ss << v;
				dst = ss.str();
			}

			std::map<std::string, std::string> store;

		public:

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
				ConvertUint64ToString(strKey, key);
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
				ConvertUint64ToString(strKey, key);
				return Get(strKey, &value);
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
	};


} // namespace KVStore

#endif
