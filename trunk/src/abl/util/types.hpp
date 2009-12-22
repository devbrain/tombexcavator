#ifndef __ABL_UTIL_TYPES_HPP__
#define __ABL_UTIL_TYPES_HPP__

#include <cstddef>
#include <sys/types.h>

#include "abl/platform.h"
#include "abl/util/c_types.h"
#include "abl/util/static_assert.hpp"

#if !defined (TE_PLATFORM_WORD_SIZE)
#error "Undefined TE_PLATFORM_WORD_SIZE"
#endif

namespace types
{
  typedef te_int8_t   sint8_t; /**< signed 8 bit integer */
  typedef te_uint8_t  uint8_t; /**< unsigned 8 bit integer */
  
  typedef te_int16_t  sint16_t; /**< signed 16 bit integer */
  typedef te_uint16_t uint16_t; /**< unsigned 16 bit integer */

  typedef te_int32_t   sint32_t; /**< signed 32 bit integer */
  typedef te_uint32_t  uint32_t; /**< unsigned 32 bit integer */
  
  typedef te_int64_t   sint64_t; /**< signed 64 bit integer */
  typedef te_uint64_t  uint64_t; /**< unsigned 64 bit integer */
  
  typedef off_t          file_size_t;
  
#if (TE_PLATFORM_WORD_SIZE == 8)
  typedef sint8_t  ssize_t;
#endif
  
#if (TE_PLATFORM_WORD_SIZE == 16)
  typedef sint16_t  ssize_t;
#endif
  
#if (TE_PLATFORM_WORD_SIZE == 32)
  typedef sint32_t  ssize_t;
#endif

}


STATIC_ASSERT (sizeof (types::sint8_t) == 1);
STATIC_ASSERT (sizeof (types::uint8_t) == 1);

STATIC_ASSERT (sizeof (types::sint16_t) == 2);
STATIC_ASSERT (sizeof (types::uint16_t) == 2);

STATIC_ASSERT (sizeof (types::sint32_t) == 4);
STATIC_ASSERT (sizeof (types::uint32_t) == 4);

STATIC_ASSERT (sizeof (types::sint64_t) == 8);
STATIC_ASSERT (sizeof (types::uint64_t) == 8);

#endif
