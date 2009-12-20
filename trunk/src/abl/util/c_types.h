/**
 * @file   types.h
 * @author igor
 * @date   Mon Sep  8 22:45:57 2008
 * 
 * @brief  provides types definitions
 * 
 */

#ifndef __ABL_C_TYPES_H__
#define __ABL_C_TYPES_H__

#include "abl/platform.h"
#include "abl/util/c_static_assert.h"


/* ------------------------------------------------------------ */
/* Determine platform depended word type                        */
/* ------------------------------------------------------------ */

typedef unsigned char te_uint8_t;
typedef signed   char te_int8_t;

#if defined (TE_PLATFORM_WORD_SIZE) && (TE_PLATFORM_WORD_SIZE > 16)
#if defined (TE_COMPILER_MSVC) || defined (TE_COMPILER_BORLAND) 
typedef __int64          te_qword_t;
typedef unsigned __int64 te_quword_t;
#endif

#if defined (TE_COMPILER_MINGW) || defined (TE_COMPILER_GCC) || defined (TE_COMPILER_SUNCC)
typedef long long          te_qword_t;
typedef unsigned long long te_quword_t;
#endif

#endif

#if defined (TE_PLATFORM_WORD_SIZE)
#if (TE_PLATFORM_WORD_SIZE == 16)       /* 16 bit architecture */
typedef unsigned short   te_word_t;     /**< represents machine word */
#define TE_MAX_WORD 0xFFFFU
typedef unsigned short   te_uint16_t;
typedef signed   short   te_int16_t;

typedef unsigned long    te_uint32_t;
typedef signed   long    te_int32_t;

typedef te_int32_t    te_qword_t;
typedef te_uint32_t   te_quword_t;

typedef te_uint32_t   TE_MAX_uint_t;
typedef te_int32_t    TE_MAX_int_t;

#elif (TE_PLATFORM_WORD_SIZE == 32)  /* 32 bit architecture */
typedef unsigned int   te_word_t;    /**< represents machine word */
#define TE_MAX_WORD 0xFFFFFFFFUL

typedef unsigned short      te_uint16_t;
typedef signed   short      te_int16_t;

typedef unsigned int        te_uint32_t;
typedef signed   int        te_int32_t;

typedef te_quword_t      te_uint64_t;
typedef te_qword_t       te_int64_t;

typedef te_uint64_t      TE_MAX_uint_t;
typedef te_int64_t       TE_MAX_int_t;


#elif (TE_PLATFORM_WORD_SIZE == 64) /* 64 bit architecture */

typedef size_t  te_word_t;     /**< represents machine word */
#define TE_MAX_WORD 0xFFFFFFFFFFFFFFFFULL;

typedef unsigned short      te_uint16_t;
typedef signed   short      te_int16_t;

typedef unsigned int        te_uint32_t;
typedef signed   int        te_int32_t;

typedef te_quword_t      te_uint64_t;
typedef te_qword_t       te_int64_t;

#else
#error "TE_PLATFORM_WORD_SIZE not handeled"
#endif

#else
#error "TE_PLATFORM_WORD_SIZE is undefined. See cache/platform.h"
#endif

/* ------------------------------------------------------------ */
/* Determine platform depended word type                        */
/* ------------------------------------------------------------ */

#if defined(TE_HAVE_SIZE_T)

#include <stddef.h>

typedef size_t te_size_t;     /**< represents size in bytes */

#else
#if (TE_PLATFORM_WORD_SIZE == 16)       /* 16 bit architecture */
typedef te_uint16_t te_size_t;
#elif (TE_PLATFORM_WORD_SIZE == 32)       /* 32 bit architecture */
typedef te_uint32_t te_size_t;
#elif (TE_PLATFORM_WORD_SIZE == 64)       /* 64 bit architecture */
typedef te_uint64_t te_size_t;
#endif
#endif

#if defined(TE_HAVE_SSIZE_T)
#include <stddef.h>

typedef ssize_t te_ssize_t;     /**< represents size in bytes */

#else
#if (TE_PLATFORM_WORD_SIZE == 16)       /* 16 bit architecture */
typedef te_int16_t te_ssize_t;
#elif (TE_PLATFORM_WORD_SIZE == 32)       /* 32 bit architecture */
typedef te_int32_t te_ssize_t;
#elif (TE_PLATFORM_WORD_SIZE == 64)       /* 64 bit architecture */
typedef te_int64_t te_ssize_t;
#endif
#endif

/* ------------------------------------------------------------ */

#define TE_NULL (void*)0     /**< null pointer */

#if defined(BOOL) 
typedef BOOL te_bool_t;      /**< boolean type */
#else
typedef unsigned te_bool_t;
#endif

#if defined(TRUE)
#define TE_TRUE  TRUE
#else
#define TE_TRUE  1
#endif

#if defined(FALSE)
#define TE_FALSE FALSE
#else
#define TE_FALSE 0
#endif

# if defined (TE_OS_WINDOWS)
#  if defined (TE_COMPILER_MINGW)
#   define UINT64_LITERAL(n) n ## ull
#   define INT64_LITERAL(n) n ## ll
#  else
#   define UINT64_LITERAL(n) n ## ui64
#   define INT64_LITERAL(n) n ## i64
#  endif /* defined (__MINGW32__) */
# elif defined (__TANDEM)
#   define UINT64_LITERAL(n) n ## LL
#   define INT64_LITERAL(n) n ## LL
# else  /* ! ACE_WIN32  */
#   define UINT64_LITERAL(n) n ## ull
#   define INT64_LITERAL(n) n ## ll
# endif /* ! WIN32  */

#define TE_INT8_MAX			0x7F
#define TE_INT8_MIN			(-(TE_INT8_MAX)-1)
#define TE_UINT8_MAX		0xFF
#define TE_INT16_MAX		0x7FFF
#define TE_INT16_MIN		(-(TE_INT16_MAX)-1)
#define TE_UINT16_MAX		0xFFFF
#define TE_INT32_MAX		0x7FFFFFFF
#define TE_INT32_MIN		(-(TE_INT32_MAX)-1)
#define TE_UINT32_MAX		0xFFFFFFFF
#define TE_INT64_MAX		INT64_LITERAL(0x7FFFFFFFFFFFFFFF)
#define TE_INT64_MIN		(-(TE_INT64_MAX)-1)


/* -------------------------------------------------- */
/* Make sure the types really have the right sizes    */
/* -------------------------------------------------- */

C_STATIC_ASSERT (sizeof_ptr   , sizeof (void*) == sizeof (te_size_t));
C_STATIC_ASSERT (te_word_t , TE_PLATFORM_BYTE_SIZE*sizeof (te_word_t) == TE_PLATFORM_WORD_SIZE);
C_STATIC_ASSERT (sizeof_ulong , TE_PLATFORM_BYTE_SIZE*sizeof (unsigned long) >= 32);
C_STATIC_ASSERT (sizeof_quword, TE_PLATFORM_BYTE_SIZE*sizeof (te_quword_t) == 64);



C_STATIC_ASSERT (sizeof_u8  , sizeof (te_uint8_t) == 1);
C_STATIC_ASSERT (sizeof_i8  , sizeof (te_int8_t ) == 1);

C_STATIC_ASSERT (sizeof_u16  , sizeof (te_uint16_t) == 2);
C_STATIC_ASSERT (sizeof_i16  , sizeof (te_int16_t ) == 2);

C_STATIC_ASSERT (sizeof_u32  , sizeof (te_uint32_t) == 4);
C_STATIC_ASSERT (sizeof_i32  , sizeof (te_int32_t ) == 4);

#if (TE_PLATFORM_WORD_SIZE != 16) 
C_STATIC_ASSERT (sizeof_u64  , sizeof (te_uint64_t) == 8);
C_STATIC_ASSERT (sizeof_i64  , sizeof (te_int64_t ) == 8);

C_STATIC_ASSERT (sizeof_imax  , sizeof (TE_MAX_int_t ) == 8);
C_STATIC_ASSERT (sizeof_umax  , sizeof (TE_MAX_uint_t) == 8);
#else
C_STATIC_ASSERT (sizeof_imax  , sizeof (TE_MAX_int_t ) == 4);
C_STATIC_ASSERT (sizeof_umax  , sizeof (TE_MAX_uint_t) == 4);
#endif

C_STATIC_ASSERT (sizeof_size_t , 8*sizeof (te_size_t) == TE_PLATFORM_WORD_SIZE);
C_STATIC_ASSERT (sizeof_ssize_t, 8*sizeof (te_ssize_t) == TE_PLATFORM_WORD_SIZE);


#endif
