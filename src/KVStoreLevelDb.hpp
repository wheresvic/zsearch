#ifndef KVSTORE_LEVELDB_H
#define KVSTORE_LEVELDB_H

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>
#include "IKVStore.h"
#include "leveldb/write_batch.h"
#include "ZUtil.hpp"


namespace KVStore
{
	class KVStoreLevelDb : public IKVStore
	{
		private:

			leveldb::DB* db;
			const std::string path;
			leveldb::WriteBatch batch;
			
		public:

			KVStoreLevelDb(const std::string& path) : path(path)
			{
				db = NULL;

				leveldb::Options options;
				leveldb::DestroyDB(path, options);
			}

			~KVStoreLevelDb()
			{
				delete db;
			}

			Status Open()
			{
				leveldb::Options options;
				// options.block_cache = leveldb::NewLRUCache(1024*128);
				// options.write_buffer_size =  1024 * 128; // 16777216; // 16Mb
				options.create_if_missing = true;
				leveldb::Status status = leveldb::DB::Open(options, path, &db);
				return Status::OK();
			}

			Status Put(const std::string& key, const std::string& value)
			{
				leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();

			}

			Status Put(uint64_t key, const std::string& value)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				return Put(keystr, value);
			}

			Status Get(const std::string& key, std::string* value)
			{
				leveldb::Status s = db->Get(leveldb::ReadOptions(), key, value);
				if (s.ok())
				{
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
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				return Get(keystr, &value);
			}

			Status Delete(uint64_t key)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				return Delete(keystr);
			}
			
			Status Delete(const std::string& key)
			{
				leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();
			}

	      	void Compact()
	      	{
		        db->CompactRange(NULL,NULL);
			}

			Status Put(const std::vector<std::pair<unsigned int, std::string>>& writes)
			{
				leveldb::WriteBatch batch;

				for (auto kv : writes)
				{
					std::string keystr;
					ZUtil::PutVarint64(keystr, kv.first);
					batch.Put(keystr, kv.second);
				}

				leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();

			}

			Status Put(const std::vector<std::pair<std::string, std::string>>& writes)
			{
				leveldb::WriteBatch batch;

				for (auto kv : writes)
				{
					batch.Put(kv.first, kv.second);
				}

				leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();

			}

			void PutBatch(const std::string& key, const std::string& value)
			{
				batch.Put(key, value);
			}

			void PutBatch(uint64_t key, const std::string& value)
			{
				string keystr;
				ZUtil::PutVarint64(keystr, key);
				batch.Put(keystr, value);	
			}

			void DeleteBatch(const std::string& key)
			{
				batch.Delete(key);
			}

			void ClearBatch()
			{
				batch.Clear();
			}

			Status writeBatch()
			{
				leveldb::Status s = db->Write(leveldb::WriteOptions(), &batch);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::IOError();
			}		
	};

} // namespace KVStore

#endif