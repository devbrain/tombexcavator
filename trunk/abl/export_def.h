/**
 * @file   export.h
 * @author igor
 * @date   Mon Sep 22 09:28:54 2008
 * 
 * @brief  This file sets things up for C dynamic library function definitions and 
 *         static inlined functions
 */

#ifndef __ABL_EXPORT_H__
#define __ABL_EXPORT_H__

#include "thirdparty/predef/predef.h"
/*  Set up for C function definitions, even when using C++ */

#ifdef __cplusplus
#define CPP_VISIBLE_BEGIN extern "C" {
#define CPP_VISIBLE_END }
#else
#define CPP_VISIBLE_BEGIN
#define CPP_VISIBLE_END 
#endif

#if defined(_MSC_VER)
#pragma warning( disable: 4251 )
#endif

/* Some compilers use a special export keyword */

#ifndef DECLSPEC
# if defined(__BEOS__)
#  if defined(__GNUC__)
#   define te__api_export__		__declspec(dllexport)
#  else
#   define te__api_export__		__declspec(export)
#  endif
# elif defined(PREDEF_OS_WINDOWS)
#  ifdef __BORLANDC__
#    define te__api_export__ 
#    define te__api_import__ 	__declspec(dllimport)
#  else
#   define te__api_export__		__declspec(dllexport)
#   define te__api_import__		__declspec(dllimport)
#  endif
# elif defined(__OS2__)
#  ifdef __WATCOMC__
#    define te__api_export__	__declspec(dllexport)
#    define te__api_import__
#  else
#   define te__api_export__
#   define te__api_import__
#  endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define te__api_export__		__attribute__ ((visibility("default")))
#   define te__api_import__		__attribute__ ((visibility("default")))
#  else
#   define te__api_export__
#   define te__api_import__
#  endif
# endif
#endif

/* Use C calling convention by default*/

#ifndef te__callspec__
#if defined(__WIN32__) && !defined(__GNUC__)
#define te__callspec__	 __cdecl
#else
#ifdef __OS2__
/* But on OS/2, we use the _System calling convention */
/* to be compatible with every compiler */
#define te__callspec__ _System
#else
#define te__callspec__
#endif
#endif
#endif /* te__callspec__*/

#ifdef __SYMBIAN32__ 
#	ifndef EKA2 
#		undef te__api_export__
#		undef te__api_import__
#		define te__api_export__
#		define te__api_import__
#	elif !defined(__WINS__)
#		undef te__api_export__
#		undef te__api_import__
#		define te__api_export__ __declspec(dllexport)
#		define te__api_import__ __declspec(dllexport)
#	endif /* !EKA2 */
#endif /* __SYMBIAN32__ */


#if defined(__GNUC__) && (__GNUC__ > 2)
#define TE_EXPECT_CONDITIONAL(c)    (__builtin_expect((c), 1))
#define TE_UNEXPECT_CONDITIONAL(c)  (__builtin_expect((c), 0))
#else
#define TE_EXPECT_CONDITIONAL(c)    (c)
#define TE_UNEXPECT_CONDITIONAL(c)  (c)
#endif


/* Set up compiler-specific options for inlining functions */
#ifndef TE_HAS_INLINE
#if defined(__GNUC__) || defined(__POCC__) || defined(__WATCOMC__) || defined(__SUNPRO_C)
#define TE_HAS_INLINE
#else
/* Add any special compiler-specific cases here */
#if defined(_MSC_VER) || defined(__BORLANDC__) ||	\
  defined(__DMC__) || defined(__SC__) ||		\
  defined(__WATCOMC__) || defined(__LCC__) ||		\
  defined(__DECC) || defined(__EABI__)
#ifndef __inline__
#define __inline__	__inline
#endif
#define TE_HAS_INLINE
#else
#if !defined(__MRC__) && !defined(_SGI_SOURCE)
#ifndef __inline__
#define __inline__ inline
#endif
#define TE_HAS_INLINE
#endif /* Not a funky compiler */
#endif /* Visual C++ */
#endif /* GNU C */
#endif /* CACHE_HAS_INLINE */

/* If inlining isn't supported, remove "__inline__", turning static
   inlined functions into static functions (resulting in code bloat
   in all files which include the offending header files)
*/
#ifndef TE_HAS_INLINE
#define __inline__
#endif


#endif
