#ifndef __PROVIDER_EXPORT_HPP__
#define __PROVIDER_EXPORT_HPP__

#include "abl/export_def.h"

#if defined(LIBPROVIDER_AS_STATIC_LIB)
#   define LIBPROVIDER_API
#else
#   if defined(BUILD_LIBPROVIDER)
#      define LIBPROVIDER_API te__api_export__ 
#   else
#      define LIBPROVIDER_API te__api_import__ 
#   endif
#endif

#if !defined(INLINE_API)
#define INLINE_API LIBPROVIDER_API
#endif

#endif
