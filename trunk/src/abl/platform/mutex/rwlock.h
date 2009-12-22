#ifndef __ABL_PLATFORM_MUTEX_RWLOCK_H__
#define __ABL_PLATFORM_MUTEX_RWLOCK_H__

#include "abl/abl_export.h"
#include "abl/util/c_types.h"

CPP_VISIBLE_BEGIN

typedef struct _rwlock_s rwlock_s;

rwlock_s* rwlock_create         (void);
void      rwlock_read_lock      (rwlock_s* rwlock);
te_bool_t rwlock_read_try_lock  (rwlock_s* rwlock);
void      rwlock_write_lock     (rwlock_s* rwlock);
te_bool_t rwlock_write_try_lock (rwlock_s* rwlock);
void      rwlock_unlock         (rwlock_s* rwlock);  
void      rwlock_destroy        (rwlock_s* rwlock);

CPP_VISIBLE_END

#endif
