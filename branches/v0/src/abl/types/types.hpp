#ifndef __ABL_UTIL_TYPES_HPP__
#define __ABL_UTIL_TYPES_HPP__

#include <cstddef>
#include <sys/types.h>

#include "abl/platform.h"
#include "abl/util/static_assert.hpp"

#define __INCLUDE_BASIC_TYPES__
#include "abl/types/basic_types.hpp"

#if defined(_MSC_VER)
#undef ABL_HAVE_INT8_T
#undef ABL_HAVE_UINT8_T
#undef ABL_HAVE_INT16_T
#undef ABL_HAVE_UINT16_T
#undef ABL_HAVE_INT32_T
#undef ABL_HAVE_UINT32_T
#undef ABL_HAVE_INT64_T
#undef ABL_HAVE_UINT64_T
#undef ABL_HAVE_SSIZE_T

#else
// GCC
#define ABL_HAVE_INT8_T
#define ABL_HAVE_UINT8_T
#define ABL_HAVE_INT16_T
#define ABL_HAVE_UINT16_T
#define ABL_HAVE_INT32_T
#define ABL_HAVE_UINT32_T
#define ABL_HAVE_INT64_T
#define ABL_HAVE_UINT64_T
#define ABL_HAVE_SSIZE_T
#include <stdint.h>
#endif

#if !defined (ABL_HAVE_INT8_T) && !defined(_CHAR_IS_SIGNED)
typedef Int8	int8_t;
#endif

#if !defined (ABL_HAVE_UINT8_T)
typedef UInt8	uint8_t;
#endif

#if !defined (ABL_HAVE_INT16_T)
typedef Int16	int16_t;
#endif

#if !defined (ABL_HAVE_UINT16_T)
typedef UInt16	uint16_t;
#endif

#if !defined (ABL_HAVE_INT32_T)
typedef Int32	int32_t;
#endif

#if !defined (ABL_HAVE_UINT32_T)
typedef UInt32	uint32_t;
#endif

#if !defined (ABL_HAVE_INT64_T)
typedef Int64	int64_t;
#endif

#if !defined (ABL_HAVE_UINT64_T)
typedef UInt64	uint64_t;
#endif

#if !defined (ABL_HAVE_SSIZE_T)
#if defined(_WIN32)
typedef int32_t ssize_t;
#elif defined(_WIN64)
typedef int64_t ssize_t;
#endif

#endif


STATIC_ASSERT (sizeof (int8_t) == 1);
STATIC_ASSERT (sizeof (uint8_t) == 1);

STATIC_ASSERT (sizeof (int16_t) == 2);
STATIC_ASSERT (sizeof (uint16_t) == 2);

STATIC_ASSERT (sizeof (int32_t) == 4);
STATIC_ASSERT (sizeof (uint32_t) == 4);

STATIC_ASSERT (sizeof (int64_t) == 8);
STATIC_ASSERT (sizeof (uint64_t) == 8);


#endif
