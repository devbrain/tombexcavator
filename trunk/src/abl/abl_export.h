#ifndef __ABL_EXPORT_HPP__
#define __ABL_EXPORT_HPP__

#include "abl/export_def.h"

#if defined(ABL_AS_STATIC_LIB)
#   define ABL_API
#else
#   if defined(BUILD_ABL)
#      define ABL_API te__api_export__ 
#   else
#      define ABL_API te__api_import__ 
#   endif
#endif

#if !defined (INLINE_API)
#define INLINE_API ABL_API
#endif

#endif
