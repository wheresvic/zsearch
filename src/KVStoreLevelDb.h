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

			leveldb::DB* db;		

		public:
			
			char* EncodeVarint64(char* dst, uint64_t v);
			
			void PutVarint64(std::string& dst, uint64_t v);
			
			KVStoreLevelDb(const std::string& path);

			~KVStoreLevelDb();
			
			Status Open();
			
			Status Put(const std::string& key,const std::string& value);
			
			Status Put(uint64_t key,const std::string& value);
			
			Status Get(const std::string& key, std::string* value);
			
			Status Get(const std::string& key, std::string& value);
			
			Status Get(uint64_t key, std::string& value);
			
			Status Delete(const std::string& key);
			
			void Compact();
			
			
	};


} // namespace KVStore

#endif
