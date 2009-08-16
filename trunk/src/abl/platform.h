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

#endif
