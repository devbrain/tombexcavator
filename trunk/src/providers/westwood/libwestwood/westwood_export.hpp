#ifndef __WESTWOOD_EXPORT_HPP__
#define __WESTWOOD_EXPORT_HPP__

#include "abl/export_def.h"

#if defined(WESTWOOD_AS_STATIC_LIB)
#   define VFS_API
#else
#   if defined(WESTWOOD_DUNE2)
#      define WESTWOOD_API te__api_export__ 
#   else
#      define WESTWOOD_API te__api_import__ 
#   endif
#endif

#if !defined (INLINE_API)
#define INLINE_API WESTWOOD_API
#endif

#endif
