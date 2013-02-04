#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/write_batch.h"
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include "KVStoreLevelDb.h"
#include "ZUtil.hpp"

namespace KVStore
{
	        KVStoreLevelDb::KVStoreLevelDb(const std::string& path) : IKVStore(path)
			{
				db = NULL;

				leveldb::Options options;
				leveldb::DestroyDB(path, options);
			}

			KVStoreLevelDb::~KVStoreLevelDb()
			{
				delete db;
			}

			Status KVStoreLevelDb::Open()
			{
				leveldb::Options options;
				options.write_buffer_size = 16777216; // 16Mb
				options.create_if_missing = true;
				leveldb::Status status = leveldb::DB::Open(options, path, &db);
				return Status::OK();
			}

			Status KVStoreLevelDb::Put(const std::string& key, const std::string& value)
			{
				leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();

			}

			Status KVStoreLevelDb::Put(uint64_t key, const std::string& value)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				return Put(keystr, value);
			}

			Status KVStoreLevelDb::Get(const std::string& key, std::string* value)
			{
				leveldb::Status s = db->Get(leveldb::ReadOptions(), key, value);
				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();
			}

			Status KVStoreLevelDb::Get(const std::string& key, std::string& value)
			{
				return Get(key, &value);
			}

			Status KVStoreLevelDb::Get(uint64_t key, std::string& value)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				return Get(keystr, &value);
			}

			Status KVStoreLevelDb::Delete(uint64_t key)
			{
				std::string keystr;
				ZUtil::PutVarint64(keystr, key);
				return Delete(keystr);
			}
			
			Status KVStoreLevelDb::Delete(const std::string& key)
			{
				leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::NotFound();
			}


	      	void KVStoreLevelDb::Compact()
	      	{
		        db->CompactRange(NULL,NULL);
			}

			Status KVStoreLevelDb::Put(const std::vector<std::pair<unsigned int, std::string>>& writes)
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


			Status KVStoreLevelDb::Write(KVStoreLevelDBBatch& batch)
			{
				leveldb::Status s = db->Write(leveldb::WriteOptions(), &(batch.batch));

				if (s.ok())
				{
					return Status::OK();
				}

				return Status::IOError();
			}


} // namespace KVStore

