/**
 * @file   types.h
 * @author Igor <igor.gutnik@gmail.com>
 * @date   Wed Aug 11 13:18:25 2010
 * 
 * @brief  integral types
 * 
 * 
 */

#ifndef __PLATFORM_TYPES_H__
#define __PLATFORM_TYPES_H__


#include <stddef.h>

#include "platform/config.h"
#include "platform/static_assert.h"


/**
   HELPER MACRO
 */
#define __TE_DECLARE_TYPES(SB, UB, SW, UW, SDW, UDW, SLL, ULL)	\
  typedef SB int8_t;						\
  typedef UB uint8_t;						\
  typedef SW int16_t;						\
  typedef UW uint16_t;						\
  typedef SDW int32_t;						\
  typedef UDW uint32_t;						\
  typedef SLL int64_t;						\
  typedef ULL uint64_t

#if defined(TE_HAVE_STDINT_H)
#include <stdint.h>
#elif defined(TE_HAVE_SDL)
#include <SDL.h>
#if defined(_STDINT_H)
__TE_DECLARE_TYPES (Sint8 , Uint8, 
		    Sint16, Uint16, 
		    Sint32, Uint32, 
		    Sint64, Uint64);
#endif
#elif defined(TE_HAVE_GLIB)
#include <glib.h>
#if !defined(_STDINT_H)
__TE_DECLARE_TYPES (gint8 , guint8, 
		    gint16, guint16, 
		    gint32, guint32, 
		    gint64, guint64);
#endif
#else
#error "Can not deduce proper platform types"
#endif


STATIC_ASSERT (INT8,   (sizeof (int8_t  ) == 1));
STATIC_ASSERT (UINT8,  (sizeof (uint8_t ) == 1));
STATIC_ASSERT (INT16,  (sizeof (int16_t ) == 2));
STATIC_ASSERT (UINT16, (sizeof (uint16_t) == 2));
STATIC_ASSERT (INT32,  (sizeof (int32_t ) == 4));
STATIC_ASSERT (UINT32, (sizeof (uint32_t) == 4));
STATIC_ASSERT (INT64,  (sizeof (int64_t ) == 8));
STATIC_ASSERT (UINT54, (sizeof (uint64_t) == 8));



#endif
