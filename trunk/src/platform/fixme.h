#ifndef __PLATFORM_FIXME_H__
#define __PLATFORM_FIXME_H__

#include "platform/macros.h"

/*
------------------------------------------------------------------------
 FIXMEs / TODOs / NOTE macros
------------------------------------------------------------------------
*/

#define _QUOTE(x) # x
#define QUOTE(x) _QUOTE(x)
#define __FILE__LINE__ __FILE__ "(" QUOTE(__LINE__) ") : "

#if defined (_MSC_VER)
#define DO_PRAGMA(x) __pragma (x)
#define DO_MESSAGE(x) DO_PRAGMA (message (__FILE__LINE__ #x))
#define DO_WARN(x) DO_PRAGMA (message (__FILE__LINE__ #x))

#define todo(x)   DO_MESSAGE("TODO: " x) 
#define fixme(x)  DO_WARN("FIXME: " x)
#define note(x)   DO_MESSAGE("NOTE: " x)

#else
#define DO_PRAGMA(x) _Pragma (#x)
#define DO_MESSAGE(x) DO_PRAGMA (message x)
#define todo(x)  DO_MESSAGE("TODO: " x)
#define fixme(x) DO_MESSAGE("FIXME: " x)
#define note(x)  DO_MESSAGE("NOTE: " x)
#endif 

#endif
