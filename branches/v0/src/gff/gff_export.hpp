#ifndef __GFF_EXPORT_HPP__
#define __GFF_EXPORT_HPP__

#include "abl/export_def.h"

#if defined(GFF_AS_STATIC_LIB)
#   define GFF_API
#else
#   if defined(BUILD_GFF)
#      define GFF_API te__api_export__ 
#   else
#      define GFF_API te__api_import__ 
#   endif
#endif

#if !defined (INLINE_API)
#define INLINE_API GFF_API
#endif

#endif
