#ifndef __PLATFORM_EXPORT_H__
#define __PLATFORM_EXPORT_H__

#include "platform/proper_export.h"

#if defined(PLATFORM_AS_STATIC_LIB)
#   define PLATFORM_API
#else
#   if defined(BUILD_PLATFORM)
#      define PLATFORM_API DECLSPEC_EXPORT
#   else
#      define PLATFORM_API DECLSPEC_IMPORT
#   endif
#endif


#endif
