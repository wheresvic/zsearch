
/**
 * Since we're not using boost, this utility class is to help us get some basic stuff done :)
 */

#ifndef ZUTIL_HPP
#define ZUTIL_HPP

#include <string>
#include <sstream>
#include <chrono>

#include "leveldb/slice.h"
using namespace std;


class ZUtil
{
	public:

		static char* EncodeVarint64(char* dst, uint64_t v)
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

		static void PutVarint64(std::string& dst, uint64_t v)
		{
			char buf[10];
			char* ptr = EncodeVarint64(buf, v);
			dst.append(buf, ptr - buf);
		}


// for document impl
		static char* EncodeVarint32(char* dst, uint32_t v) {
		  // Operate on characters as unsigneds
		  unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
		  static const int B = 128;
		  if (v < (1<<7)) {
		    *(ptr++) = v;
		  } else if (v < (1<<14)) {
		    *(ptr++) = v | B;
		    *(ptr++) = v>>7;
		  } else if (v < (1<<21)) {
		    *(ptr++) = v | B;
		    *(ptr++) = (v>>7) | B;
		    *(ptr++) = v>>14;
		  } else if (v < (1<<28)) {
		    *(ptr++) = v | B;
		    *(ptr++) = (v>>7) | B;
		    *(ptr++) = (v>>14) | B;
		    *(ptr++) = v>>21;
		  } else {
		    *(ptr++) = v | B;
		    *(ptr++) = (v>>7) | B;
		    *(ptr++) = (v>>14) | B;
		    *(ptr++) = (v>>21) | B;
		    *(ptr++) = v>>28;
		  }
		  return reinterpret_cast<char*>(ptr);
		}
		
		static void PutVarint32(std::string* dst, uint32_t v) {
		  char buf[5];
		  char* ptr = EncodeVarint32(buf, v);
		  dst->append(buf, ptr - buf);
		}
		
		static void PutLengthPrefixedSlice(std::string* dst, const leveldb::Slice& value) {
		  PutVarint32(dst, value.size());
		  dst->append(value.data(), value.size());
		}
		
		static const char* GetVarint32PtrFallback(const char* p,
		                                   const char* limit,
		                                   uint32_t* value) {
		  uint32_t result = 0;
		  for (uint32_t shift = 0; shift <= 28 && p < limit; shift += 7) {
		    uint32_t byte = *(reinterpret_cast<const unsigned char*>(p));
		    p++;
		    if (byte & 128) {
		      // More bytes are present
		      result |= ((byte & 127) << shift);
		    } else {
		      result |= (byte << shift);
		      *value = result;
		      return reinterpret_cast<const char*>(p);
		    }
		  }
		  return NULL;
		}
		
		static inline const char* GetVarint32Ptr(const char* p,
		                                  const char* limit,
		                                  uint32_t* value) {
		  if (p < limit) {
		    uint32_t result = *(reinterpret_cast<const unsigned char*>(p));
		    if ((result & 128) == 0) {
		      *value = result;
		      return p + 1;
		    }
		  }
		  return GetVarint32PtrFallback(p, limit, value);
		}
		
		static bool GetVarint32(leveldb::Slice* input, uint32_t* value) {
		  const char* p = input->data();
		  const char* limit = p + input->size();
		  const char* q = GetVarint32Ptr(p, limit, value);
		  if (q == NULL) {
		    return false;
		  } else {
		    *input = leveldb::Slice(q, limit - q);
		    return true;
		  }
		}
		
		static const char* GetLengthPrefixedSlice(const char* p, const char* limit,
		                                   leveldb::Slice* result) {
		  uint32_t len;
		  p = GetVarint32Ptr(p, limit, &len);
		  if (p == NULL) return NULL;
		  if (p + len > limit) return NULL;
		  *result = leveldb::Slice(p, len);
		  return p + len;
		}
		
		static bool GetLengthPrefixedSlice(leveldb::Slice* input, leveldb::Slice* result) {
		  uint32_t len;
		  if (GetVarint32(input, &len) &&
		      input->size() >= len) {
		    *result = leveldb::Slice(input->data(), len);
		    input->remove_prefix(len);
		    return true;
		  } else {
		    return false;
		  }
		}
		
		//legacy stuff
		static string getString(uint64_t v)
		{
			std::stringstream ss;
			ss << v;
			return ss.str();
		}

		static string getString(int number)
		{
		   stringstream ss;
		   ss << number;
		   return ss.str();
		}

		static string getString(unsigned int number)
		{
		   stringstream ss;
		   ss << number;
		   return ss.str();
		}

		static string getString(unsigned long number)
		{
		   stringstream ss;
		   ss << number;
		   return ss.str();
		}

		static int getInt(const std::string& str)
		{
			int x;
			stringstream ss(str);
			ss >> x;

			if (!ss)
			{
				string msg = "Provided string is not an integer : " + str;
				throw msg;
			}

			return x;
		}

		static unsigned int getUInt(const std::string& str)
		{
			unsigned int x;
			stringstream ss(str);
			ss >> x;

			if (!ss)
			{
				string msg = "Provided string is not an unsigned integer : " + str;
				throw msg;
			}

			return x;
		}

		static string printTimeTaken(const std::chrono::nanoseconds& ns)
		{
			stringstream ss;

			if (ns.count() >= 1000000000)
			{
				ss << "function returned in " << std::chrono::duration_cast<std::chrono::seconds>(ns).count() << "s";
			}
			else if (ns.count() >= 1000000)
			{
				ss << "function returned in " << std::chrono::duration_cast<std::chrono::milliseconds>(ns).count() << "ms";
			}
			else
			{
				ss << "function returned in " << ns.count() << "ns";
			}

			return ss.str();
		}

};

#endif
