#ifndef COMMON_H__
#define COMMON_H__
#include <stdint.h>
#include <memory>
#include <limits>

#ifdef HAVE_BUILTIN_EXPECT
#define PREDICT_FALSE(x) (__builtin_expect(x, 0))
#define PREDICT_TRUE(x) (__builtin_expect(!!(x), 1))
#else
#define PREDICT_FALSE(x) x
#define PREDICT_TRUE(x) x
#endif

#ifdef _MSC_VER
typedef unsigned __int8  uint8;
#else
typedef uint8_t  uint8;
#endif
typedef char v16qi __attribute__ ((vector_size (16)));

#endif  // COMMON_H__
