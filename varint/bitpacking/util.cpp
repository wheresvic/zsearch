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
 bool divisibleby(size_t a, uint32_t x) {
     return (a % x == 0);
 }

 void checkifdivisibleby(size_t a, uint32_t x) {
     if (!divisibleby(a, x)) {
         ostringstream convert;
         convert << a << " not divisible by " << x;
         throw logic_error(convert.str());
     }
 }






#endif
