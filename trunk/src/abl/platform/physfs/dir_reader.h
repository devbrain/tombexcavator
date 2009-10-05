#ifndef __ABL_PHYSFS_DIR_READER_H__
#define __ABL_PHYSFS_DIR_READER_H__

#include "abl/abl_export.h"

CPP_VISIBLE_BEGIN

typedef struct _directory_s directory_s;
  
directory_s* directory_open      (const char* path);
const char*  directory_read_name (directory_s* dir);
void         directory_rewind    (directory_s* dir);
void         directory_close     (directory_s* dir);
  
CPP_VISIBLE_END

#endif
