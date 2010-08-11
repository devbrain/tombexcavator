#ifndef __PLATFORM_UNICODE_H__
#define __PLATFORM_UNICODE_H__

#include "platform/types.h"
#include "platform/platform_export.h"

PLATFORM_API void utf8_to_ucs4 CALLSPEC (const char *src, 
					 uint32_t   *dst, 
					 size_t      len);

PLATFORM_API void utf8_to_ucs2 CALLSPEC (const char *src, 
					 uint16_t   *dst, 
					 size_t      len);

PLATFORM_API void utf8_from_ucs4 CALLSPEC (const uint32_t *src, 
					   char *dst, 
					   size_t len);

PLATFORM_API void utf8_from_ucs2 CALLSPEC (const uint16_t *src, 
					   char *dst, 
					   size_t len);

PLATFORM_API void utf8_from_latin1 CALLSPEC (const char *src, 
					     char *dst, 
					     size_t len);

PLATFORM_API int utf8_strcasecmp CALLSPEC (const char *str1, 
					   const char *str2);

PLATFORM_API int utf8_strnicmp CALLSPEC (const char *str1, 
					 const char *str2, 
					 size_t n);

PLATFORM_API int stricmp_ascii CALLSPEC (const char *str1, const char *str2);

PLATFORM_API int strnicmp_ascii CALLSPEC (const char *str1, 
					  const char *str2, 
					  size_t n);
#endif
