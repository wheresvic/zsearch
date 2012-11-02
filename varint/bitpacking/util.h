/**
 * This is code is released under the
 * Apache License Version 2.0 http://www.apache.org/licenses/.
 *
 * (c) Daniel Lemire, http://lemire.me/en/
 * and Owen Kaser
 */

#ifndef BITPACK_UTIL
#define BITPACK_UTIL
#include "common.h"
#include <iostream> // for std::cerr 

using namespace std;

// taken from stackoverflow
#ifndef NDEBUG
#   define ASSERT(condition, message) \
    do { \
        if (! (condition)) { \
            std::cerr << "Assertion `" #condition "` failed in " << __FILE__ \
                      << " line " << __LINE__ << ": " << message << std::endl; \
            std::exit(EXIT_FAILURE); \
        } \
    } while (false)
#else
#   define ASSERT(condition, message) do { } while (false)
#endif

 __attribute__ ((const))
bool divisibleby(size_t a, uint32_t x);

void checkifdivisibleby(size_t a, uint32_t x);

 template <class T>
 __attribute__ ((const))
 bool needPaddingTo128Bits(const T * inbyte) {
     return reinterpret_cast<uintptr_t> (inbyte) & 15;
 }

 __attribute__ ((const))
 constexpr uint32_t gccbits(const uint32_t v) {
     return v == 0 ? 0 : 32 - __builtin_clz(v);
 }

 template<class iterator>
 __attribute__ ((pure))
 uint32_t maxbits(const iterator & begin, const iterator & end) {
     uint32_t accumulator = 0;
     for (iterator k = begin; k != end; ++k) {
         accumulator |= *k;
     }
     return gccbits(accumulator);
 }

 // For VariableByte codec
 template <class T>
 __attribute__ ((const))
 bool needPaddingTo32Bits(const T * inbyte) {
     return reinterpret_cast<uintptr_t> (inbyte) & 3;
 }

 template <class T>
 __attribute__ ((const))
 T * padTo32bits(T * inbyte) {
     return reinterpret_cast< T *> ((reinterpret_cast<uintptr_t> (inbyte)
             + 3) & ~3);
 }
#endif
