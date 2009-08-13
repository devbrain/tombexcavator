#ifndef __LIBPROVIDER_DIR_READER_H__
#define __LIBPROVIDER_DIR_READER_H__

#if defined(__cplusplus)
extern "C" {
#endif

  typedef struct _directory_s directory_s;
  
  directory_s* directory_open      (const char* path);
  const char*  directory_read_name (directory_s* dir);
  void         directory_rewind    (directory_s* dir);
  void         directory_close     (directory_s* dir);
  
#if defined(__cplusplus)
}
#endif


#endif
