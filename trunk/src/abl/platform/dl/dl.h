#ifndef __ABL_DL_H__
#define __ABL_DL_H__

#include "abl/abl_export.h"

CPP_VISIBLE_BEGIN

typedef struct _shared_object_s shared_object_s;

shared_object_s* shared_object_open (const char* path);

void* shared_object_get_symbol (shared_object_s* so, const char* name);

void shared_object_close (shared_object_s* so);


CPP_VISIBLE_END

#endif
 
