#include "leveldb/db.h"
#include "leveldb/cache.h"
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "KVStoreLevelDb.h"

namespace KVStore
{
	        char* KVStoreLevelDb::EncodeVarint64(char* dst, uint64_t v)
	        {
	        	static const unsigned int B = 128;
	        	unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
	        	while (v >= B)
	        	{
	        		*(ptr++) = (v & (B-1)) | B;
	        		v >>= 7;
	        	}
            
	        	*(ptr++) = static_cast<unsigned char>(v);
	        	return reinterpret_cast<char*>(ptr);
	        }
            
	        void KVStoreLevelDb::PutVarint64(std::string& dst, uint64_t v)
	        {
	        	char buf[10];
	        	char* ptr = EncodeVarint64(buf, v);
	        	dst.append(buf, ptr - buf);	
	        }

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
				options.write_buffer_size = 128000000;
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
				// cout << "PUT ... KEY: " << key << " ,Value.length(): " << value.size()<<endl;
				std::string keystr;
				PutVarint64(keystr, key);
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
				PutVarint64(keystr, key);
				return Get(keystr, &value);
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
	 
	        Status KVStoreLevelDb::Write(KVStoreLevelDBBatch batch){
		        leveldb::Status s = db->Write(leveldb::WriteOptions(), &(batch.batch));
				if (s.ok())
				{
					return Status::OK();
				}
		        return Status::IOError();
	        }
} // namespace KVStore

