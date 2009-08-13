#ifndef __LIBPROVIDER_UNICODE_H__
#define __LIBPROVIDER_UNICODE_H__

#if defined(__cplusplus)
extern "C" {
#endif

  typedef unsigned short unichar2_t;
  typedef unsigned int   unichar_t;

unichar2_t* utf8_to_utf16 (const char* str , long len,            
			   long* items_read, long* items_written);

unichar_t*  utf8_to_ucs4  (const char* str , long len,            
			   long* items_read, long* items_written);

unichar_t*  utf16_to_ucs4 (const unichar2_t* str, long len,            
			   long* items_read     , long* items_written);  

char*       utf16_to_utf8 (const unichar2_t* str, long len,            
			   long* items_read     , long* items_written);

unichar2_t* ucs4_to_utf16 (const unichar_t* str, long len,            
			   long* items_read    , long* items_written);  

char*       ucs4_to_utf8 (const unichar_t* str,  long len,            
			  long* items_read    ,  long *items_written);

#if defined(__cplusplus)
extern "C" {
#endif

#endif
