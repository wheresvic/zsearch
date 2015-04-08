
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
		
		
		static long atoi(const char *str)
		{
		    long num = 0;
		    while (*str != '\0')
		    {
		        num = (num << 1) + (num << 3) + *(str++) - 48;
		    }
		    return num;
		}

        static void PutUint64(std::string& dst, uint64_t v) {
		  char buf[numeric_limits<uint64_t>::digits10];
		  const size_t len = fbUint64ToChar(buf, v);
		  dst.append(buf, len);
		}

		// Convert integer to a human readable string and write to dst
		static uint32_t fbUint64ToChar(char *dst, uint64_t value)
		{
		    static auto const length = numeric_limits<uint64_t>::digits10;
		    auto next = length - 1;
		    static const char digits[201] =
			"0001020304050607080910111213141516171819"
			"2021222324252627282930313233343536373839"
			"4041424344454647484950515253545556575859"
			"6061626364656667686970717273747576777879"
			"8081828384858687888990919293949596979899";
		    while (value >= 100) {
				auto const i = (value % 100) * 2;
				value /= 100;
				dst[next] = digits[i + 1];
				dst[next - 1] = digits[i];
				next -= 2;
		    }

		    // Handle last 1-2 digits
		    if (value < 10) {
				dst[next] = '0' + uint32_t(value);
				return length - next;
		    } else {

		    }
		    auto i = uint32_t(value) * 2;
		    dst[next] = digits[i + 1];
		    dst[next - 1] = digits[i];
		    return length - next + 1;
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
