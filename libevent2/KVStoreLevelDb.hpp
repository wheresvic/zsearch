#ifndef KVSTORE_LEVELDB_H
#define KVSTORE_LEVELDB_H

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include <map>
#include <string>
#include <sstream>
#include <iostream>
#include "IKVStore.h"

namespace KVStore 
{

	class KVStoreLevelDb : public IKVStore
	{
		private:
		
			char* EncodeVarint64(char* dst, uint64_t v) 
			{
				static const int B = 128;
				unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
				while (v >= B) 
				{
					*(ptr++) = (v & (B-1)) | B;
					v >>= 7;
				}
				
				*(ptr++) = static_cast<unsigned char>(v);
				return reinterpret_cast<char*>(ptr);
			}

			void PutVarint64(std::string& dst, uint64_t v) 
			{
				/*
				char buf[10];
				char* ptr = EncodeVarint64(buf, v);
				dst.append(buf, ptr - buf);
				*/

				std::stringstream ss;
				ss << v;
				dst = ss.str();
				// std::cout << "converted " << dst << std::endl;

			}
			
			leveldb::DB* db;
			std::string path;
		
		public:
	  
			KVStoreLevelDb(const std::string& path) : path(path)
			{
				db = NULL;
			}
			
			~KVStoreLevelDb()
			{
				delete db;	
				leveldb::Options options;
				leveldb::DestroyDB(path, options);
			}
			
			Status Open()
			{
				leveldb::Options options;
				options.create_if_missing = true;
				leveldb::Status status = leveldb::DB::Open(options, path, &db);
				return Status::OK();	
			}
				
			Status Put(const std::string& key,const std::string& value)
			{
				leveldb::Status s = db->Put(leveldb::WriteOptions(), key, value);
				
				if (s.ok()) 
				{
					return Status::OK();	
				} 
				
				return Status::NotFound();
				
			}
			
			Status Put(uint64_t key,const std::string& value)
			{
				// cout << "PUT ... KEY: " << key << " ,Value.length(): " << value.size()<<endl;
				string keystr;
				PutVarint64(keystr, key);
				return Put(keystr, value);
			}

			Status Get(const std::string& key, std::string* value)
			{
				leveldb::Status s = db->Get(leveldb::ReadOptions(), key, value);
				if (s.ok()) 
				{
					// cout << "GET ... KEY: " << key << " ,Value.length(): " << value->size()<<endl;
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
				string keystr;
				PutVarint64(keystr, key);
				return Get(keystr, &value);
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
	};


} // namespace KVStore

#endif