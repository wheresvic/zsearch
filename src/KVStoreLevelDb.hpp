/**
 * http://leveldb.googlecode.com/svn/trunk/doc/index.html
 *
 * A database may only be opened by one process at a time.
 * The leveldb implementation acquires a lock from the operating system
 * to prevent misuse. Within a single process, the same leveldb::DB
 * object may be safely shared by multiple concurrent threads.
 * I.e., different threads may write into or fetch iterators or
 * call Get on the same database without any external synchronization
 * (the leveldb implementation will automatically do the required
 * synchronization). However other objects (like Iterator and WriteBatch)
 * may require external synchronization. If two threads share such an
 * object, they must protect access to it using their own locking protocol.
 * More details are available in the public header files.
 *
 */


#ifndef KVSTORE_LEVELDB_H
#define KVSTORE_LEVELDB_H

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include "leveldb/write_batch.h"
#include "leveldb/filter_policy.h"
#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

#include "IKVStore.h"
#include "ZException.hpp"
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

			KVStoreLevelDb(const std::string& path, bool destroy) : path(path)
			{
				db = NULL;

				if (destroy)
				{
					leveldb::Options options;
					leveldb::DestroyDB(path, options);
				}
			}

			~KVStoreLevelDb()
			{
				delete db;
				cerr << "Destroyed KVStoreLevelDb" << endl;
			}

			Status Open()
			{
				leveldb::Options options;
				
				//options.block_cache = leveldb::NewLRUCache(500 * 1048576); 
				options.write_buffer_size = 200 * 1048576; // 200MB write buffer
				options.filter_policy = leveldb::NewBloomFilterPolicy(16);
				options.create_if_missing = true;
				options.paranoid_checks = false;
				options.compression = leveldb::kSnappyCompression;

				leveldb::Status status = leveldb::DB::Open(options, path, &db);

				if (!status.ok())
				{
					// could possibly attempt to repair and open via
					// if (leveldb::RepairDB(path, options).ok()) ... if (!leveldb::DB::Open(options, path, &db).ok()) ... throw ...

					throw ZException("Failed to open leveldb database!");
				}

				return Status::OK();
			}

			Status Put(const std::string& key, const std::string& value)
			{
				leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);

				if (s.ok())
				{
					return Status::OK();
				}
                std::cerr << s.ToString() << std::endl;
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
