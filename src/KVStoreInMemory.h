#ifndef KVSTORE_IN_MEMORY_H
#define KVSTORE_IN_MEMORY_H

#include <map>
#include <string>
#include "IKVStore.h"

namespace KVStore
{

	class KVStoreInMemory : public IKVStore
	{
		private:

			void ConvertUint64ToString(std::string& dst, uint64_t v);
			std::map<std::string, std::string> store;

		public:

			KVStoreInMemory(const std::string& path);

			Status Open();

			Status Put(const std::string& key, const std::string& value);

			Status Put(uint64_t key, const std::string& value);

			Status Put(const std::vector<std::pair<unsigned int, std::string>>& writes);

			Status Get(const std::string& key, std::string* value);

			Status Get(const std::string& key, std::string& value);

			Status Get(uint64_t key, std::string& value);

			Status Delete(const std::string& key);

			void Compact();

	};


} // namespace KVStore

#endif
