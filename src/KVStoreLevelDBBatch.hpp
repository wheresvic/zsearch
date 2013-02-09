#ifndef KVSTORE_LEVELDB_BATCH_H
#define KVSTORE_LEVELDB_BATCH_H
#include <string>

#include "leveldb/write_batch.h"
#include "IKVStore.h"
#include "ZUtil.hpp"

namespace KVStore
{
	class KVStoreLevelDBBatch
	{
		private:

			const char ns;

		public:

			leveldb::WriteBatch batch;

			KVStoreLevelDBBatch(const char ns) : ns(ns)
			{ }

			~KVStoreLevelDBBatch()
			{ }

			void Put(const std::string& key, const std::string& value)
			{
				string keystr = key;
				keystr.insert(0, 1, ns);
				batch.Put(keystr, value);
			}

			void Put(uint64_t key, const std::string& value)
			{
				string keystr;
				ZUtil::PutVarint64(keystr, key);
				keystr.insert(0, 1, ns);
				batch.Put(keystr, value);
			}

			void Delete(const std::string& key)
			{
				string keystr = key;
				keystr.insert(0, 1, ns);
				batch.Delete(keystr);
			}

			void Clear()
			{
				batch.Clear();
			}

	};

} // namespace KVStore

#endif // KVSTORE_LEVELDB_BATCH_H
