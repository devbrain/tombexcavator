#ifndef __ABL_PLATFORM_H__
#define __ABL_PLATFORM_H__

#include "thirdparty/predef/predef.h"

#define TE_PLATFORM_BYTE_SIZE 8  /* in bits */
#define TE_PLATFORM_WORD_SIZE 32 /* in bits */

#define TE_HAVE_MEMSET        1
#define TE_HAVE_SIZE_T        1

/* ========================================================= */
/* Compiler settings                                         */
/* ========================================================= */

#ifdef PREDEF_COMPILER_VISUALC
	#define TE_COMPILER_MSVC
	#ifdef PREDEF_COMPILER_VISUALC_6_0
		#define TE_COMPILER_VERSION 6
	#endif
	#ifdef PREDEF_COMPILER_VISUALC_7_0
		#define TE_COMPILER_VERSION 7
	#endif
#endif

#ifdef PREDEF_COMPILER_MINGW32
	#define TE_COMPILER_MINGW
#endif

#ifdef PREDEF_COMPILER_BORLAND
	#define TE_COMPILER_BORLAND
#endif

#ifdef PREDEF_COMPILER_GCC
	#define TE_COMPILER_GCC
#endif


#ifdef PREDEF_COMPILER_SUNPRO
	#define TE_COMPILER_SUNCC
#endif

/* ========================================================= */
/* OS                                                        */
/* ========================================================= */

#if defined (PREDEF_OS_WINDOWS)
#define TE_OS_WINDOWS
#endif

#if defined (PREDEF_OS_LINUX)
#define TE_OS_LINUX
#endif

#if defined (PREDEF_OS_SOLARIS)
#define TE_OS_SOLARIS
#endif

/* ========================================================= */
/* C++                                                        */
/* ========================================================= */


#if defined(TE_NO_MEMBER_TEMPLATES)
#undef TE_NO_MEMBER_TEMPLATES 
#endif

#if !defined(TE_HAS_MUTABLE)
#define TE_HAS_MUTABLE
#endif

#if !defined(TE_HAS_EXPLICIT)
#define TE_HAS_EXPLICIT
#endif

#if defined(TE_HAS_MUTABLE)
#define __te_mutable__ mutable
#endif

#if defined(TE_HAS_EXPLICIT)
#define __te_explicit__ explicit
#endif

/* ======================================================== */
/* Endianity                                                */
/* ======================================================== */
#if defined(__ALPHA) || defined(__alpha) || defined(__alpha__) || defined(_M_ALPHA)
#define TE_ARCH_LITTLE_ENDIAN 1
#elif defined(i386) || defined(__i386) || defined(__i386__) || defined(_M_IX86)
#define TE_ARCH_LITTLE_ENDIAN 1
#elif defined(_IA64) || defined(__IA64__) || defined(__ia64__) || defined(__ia64) || defined(_M_IA64)
#if defined(hpux) || defined(_hpux)
#define TE_ARCH_BIG_ENDIAN 1
#else
#define TE_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__x86_64__) || defined(_M_X64)
#define TE_ARCH_LITTLE_ENDIAN 1
#elif defined(__mips__) || defined(__mips) || defined(__MIPS__) || defined(_M_MRX000)
#define TE_ARCH_BIG_ENDIAN 1
#elif defined(__hppa) || defined(__hppa__)
#define TE_ARCH_BIG_ENDIAN 1
#elif defined(__PPC) || defined(__POWERPC__) || defined(__powerpc) || defined(__PPC__) || \
  defined(__powerpc__) || defined(__ppc__) || defined(_ARCH_PPC) || defined(_M_PPC)
#define TE_ARCH_BIG_ENDIAN 1
#elif defined(_POWER) || defined(_ARCH_PWR) || defined(_ARCH_PWR2) || defined(_ARCH_PWR3) || \
  defined(_ARCH_PWR4) || defined(__THW_RS6000)
#define TE_ARCH_BIG_ENDIAN 1
#elif defined(__sparc__) || defined(__sparc) || defined(sparc)
#define TE_ARCH_BIG_ENDIAN 1
#elif defined(__arm__) || defined(__arm) || defined(ARM) || defined(_ARM_) || defined(__ARM__) || defined(_M_ARM)
#if defined(__ARMEB__)
#define TE_ARCH_BIG_ENDIAN 1
#else
#define TE_ARCH_LITTLE_ENDIAN 1
#endif
#elif defined(__m68k__)
#define TE_ARCH_BIG_ENDIAN 1
#elif defined(__s390__)
#define TE_ARCH_BIG_ENDIAN 1
#endif

#endif
