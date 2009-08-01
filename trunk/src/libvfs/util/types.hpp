#ifndef __LIBVFS_UTIL_TYPES_HPP__
#define __LIBVFS_UTIL_TYPES_HPP__

#include <cstddef>
#include <sys/types.h>

#include "libvfs/util/static_assert.hpp"

namespace util
{
  typedef signed   char  sint8_t; /**< signed 8 bit integer */
  typedef unsigned char  uint8_t; /**< unsigned 8 bit integer */
  
  typedef signed   short sint16_t; /**< signed 16 bit integer */
  typedef unsigned short uint16_t; /**< unsigned 16 bit integer */

  typedef signed   int   sint32_t; /**< signed 32 bit integer */
  typedef unsigned int   uint32_t; /**< unsigned 32 bit integer */
  
  typedef off_t          file_size_t;
  typedef int            ssize_t;
}


STATIC_ASSERT (sizeof (util::sint8_t) == 1);
STATIC_ASSERT (sizeof (util::uint8_t) == 1);

STATIC_ASSERT (sizeof (util::sint16_t) == 2);
STATIC_ASSERT (sizeof (util::uint16_t) == 2);

STATIC_ASSERT (sizeof (util::sint32_t) == 4);
STATIC_ASSERT (sizeof (util::uint32_t) == 4);

#endif
