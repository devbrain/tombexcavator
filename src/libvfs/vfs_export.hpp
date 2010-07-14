#ifndef __VFS_EXPORT_HPP__
#define __VFS_EXPORT_HPP__

#include "abl/export_def.h"

#if defined(VFS_AS_STATIC_LIB)
#   define VFS_API
#else
#   if defined(BUILD_VFS)
#      define VFS_API te__api_export__ 
#   else
#      define VFS_API te__api_import__ 
#   endif
#endif

#if !defined (INLINE_API)
#define INLINE_API VFS_API
#endif

#endif
