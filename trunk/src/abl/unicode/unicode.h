#ifndef __ABL_UNICODE_H__
#define __ABL_UNICODE_H__

#include "abl/abl_export.h"
#include "abl/util/c_types.h"

CPP_VISIBLE_BEGIN

typedef te_uint16_t ucs16_t;
typedef te_uint32_t ucs4_t;

ABL_API ucs16_t* utf8_to_utf16 (const char* str , long len,            
				long* items_read, long* items_written);

ABL_API ucs4_t* utf8_to_ucs4  (const char* str , long len,            
			       long* items_read, long* items_written);

ABL_API ucs4_t* utf16_to_ucs4 (const ucs16_t* str, long len,            
			       long* items_read     , long* items_written);  

ABL_API char*  utf16_to_utf8 (const ucs16_t* str, long len,            
			      long* items_read     , long* items_written);

ABL_API ucs16_t* ucs4_to_utf16 (const ucs4_t* str, long len,            
				long* items_read    , long* items_written);  

ABL_API char*  ucs4_to_utf8 (const ucs4_t* str,  long len,            
			     long* items_read    ,  long *items_written);

CPP_VISIBLE_END

#endif
