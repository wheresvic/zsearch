#ifndef KVSTORE_LEVELDB_BATCH_H
#define KVSTORE_LEVELDB_BATCH_H
#include <string>
#include "leveldb/write_batch.h"
#include "IKVStore.h"
namespace KVStore
{
  class KVStoreLevelDBBatch {
  public:
	leveldb::WriteBatch batch;
	
  	KVStoreLevelDBBatch(){}
  	~KVStoreLevelDBBatch(){}

    char* EncodeVarint64(char* dst, uint64_t v)
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
    
    void PutVarint64(std::string& dst, uint64_t v)
    {
    	char buf[10];
    	char* ptr = EncodeVarint64(buf, v);
    	dst.append(buf, ptr - buf);	
    }

  	void Put(const std::string& key, const std::string& value)
	{
		batch.Put(key, value);
    }
	
	void Put(uint64_t key, const std::string& value)
	{
		std::string keystr;
		PutVarint64(keystr, key);
		batch.Put(keystr, value);
	}
		
	void Delete(const std::string& key)
	{
		batch.Delete(key);
	}
	
	// Clear all updates buffered in this batch.
    void Clear(){
		batch.Clear();
    }



  };
} // namespace KVStore

#endif // KVSTORE_LEVELDB_BATCH_H
