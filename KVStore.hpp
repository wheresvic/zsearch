#ifndef KVSTORE_H
#define KVSTORE_H

#include "leveldb/db.h"
#include "leveldb/cache.h"
#include <string>
namespace KVStore {
class Status {
	enum Code {
    kOk = 0,
    kNotFound = 1,
    kCorruption = 2,
    kNotSupported = 3,
    kInvalidArgument = 4,
    kIOError = 5
  };
  Code _code;
public:
  Code code() const {
    return _code;
  }

  Status(Code code){
	_code = code;
  }
  // Return a success status.
  static Status OK() { return Status(kOk); }

  // Return error status of an appropriate type.
  static Status NotFound() {
    return Status(kNotFound);
  }
  static Status Corruption() {
    return Status(kCorruption);
  }
  static Status NotSupported() {
    return Status(kNotSupported);
  }
  static Status InvalidArgument() {
    return Status(kInvalidArgument);
  }

  static Status IOError() {
    return Status(kIOError);
  }

  // Returns true iff the status indicates success.
  bool ok() const { return (code() == kOk); }

  // Returns true iff the status indicates a NotFound error.
  bool IsNotFound() const { return code() == kNotFound; }

  // Returns true iff the status indicates a Corruption error.
  bool IsCorruption() const { return code() == kCorruption; }

  // Returns true iff the status indicates an IOError.
  bool IsIOError() const { return code() == kIOError; }
};

class KVStore
{
	leveldb::DB* db;
	public:
  
		KVStore(){
			db = NULL;
		}
		
		Status Open(){
			leveldb::Options options;
		    options.create_if_missing = true;
		    leveldb::Status status = leveldb::DB::Open(options, "/tmp/testdb", &db);
			return Status::OK();	
		}
			
		Status Put(const std::string& key,const std::string& value){
			leveldb::Status s = db->Put(leveldb::WriteOptions(), key,value);
			if (s.ok()) {
				return Status::OK();	
			} else {
				return Status::NotFound();
			}
		}

		Status Get(const std::string& key, std::string* value){
			leveldb::Status s = db->Get(leveldb::ReadOptions(), key, value);
			if (s.ok()) {
				return Status::OK();	
			} else {
				return Status::NotFound();
			}
		}
		
		
		Status Delete(const std::string& key){
			leveldb::Status s = db->Delete(leveldb::WriteOptions(), key);
			if (s.ok()) {
				return Status::OK();	
			} else {
				return Status::NotFound();
			}
		}
		
		
		
		~KVStore(){
			delete db;	
		}

};


} // namespace KVStore

#endif