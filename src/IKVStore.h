#ifndef IKVSTORE_H
#define IKVSTORE_H

#include <string>

namespace KVStore
{

	class Status 
	{
		enum Code 
		{
			kOk = 0,
			kNotFound = 1,
			kCorruption = 2,
			kNotSupported = 3,
			kInvalidArgument = 4,
			kIOError = 5
		};
		
		Code _code;

		public:
			
			Code code() const 
			{
				return _code;
			}

		Status(Code code)
		{
			_code = code;
		}
		
		// Return a success status.
		static Status OK() { return Status(kOk); }

		// Return error status of an appropriate type.
		static Status NotFound() { return Status(kNotFound); }
  
		static Status Corruption() { return Status(kCorruption); }
  
		static Status NotSupported() { return Status(kNotSupported); }
  
		static Status InvalidArgument() { return Status(kInvalidArgument); }

		static Status IOError() { return Status(kIOError); }

		// Returns true iff the status indicates success.
		bool ok() const { return (code() == kOk); }

		// Returns true iff the status indicates a NotFound error.
		bool IsNotFound() const { return code() == kNotFound; }

		// Returns true iff the status indicates a Corruption error.
		bool IsCorruption() const { return code() == kCorruption; }

		// Returns true iff the status indicates an IOError.
		bool IsIOError() const { return code() == kIOError; }
	};

	
	class IKVStore
	{
		public:
			
			virtual Status Open() = 0;
			virtual Status Put(const std::string& key,const std::string& value) = 0;
			virtual Status Put(uint64_t key,const std::string& value) = 0;
			virtual Status Get(const std::string& key, std::string* value) = 0;
			virtual Status Get(const std::string& key, std::string& value) = 0;
			virtual Status Get(uint64_t key, std::string& value) = 0;
			virtual Status Delete(const std::string& key) = 0;
		
			virtual ~IKVStore() { }
	};

}

#endif
