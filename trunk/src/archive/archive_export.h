#ifndef __ARCHIVE_EXPORT_H__
#define __ARCHIVE_EXPORT_H__

#include "platform/proper_export.h"

#if defined(ARCHIVE_AS_STATIC_LIB)
#   define ARCHIVE_API
#else
#   if defined(BUILD_ARCHIVE)
#      define ARCHIVE_API TE_DECLSPEC_EXPORT
#   else
#      define ARCHIVE_API TE_DECLSPEC_IMPORT
#   endif
#endif


#endif
