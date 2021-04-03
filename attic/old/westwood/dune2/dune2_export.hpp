#ifndef __DUNE2_EXPORT_HPP__
#define __DUNE2_EXPORT_HPP__

#include "abl/export_def.h"

#if defined(DUNE2_AS_STATIC_LIB)
#   define VFS_API
#else
#   if defined(BUILD_DUNE2)
#      define DUNE2_API te__api_export__ 
#   else
#      define DUNE2_API te__api_import__ 
#   endif
#endif

#if !defined (INLINE_API)
#define INLINE_API DUNE2_API
#endif

#endif
