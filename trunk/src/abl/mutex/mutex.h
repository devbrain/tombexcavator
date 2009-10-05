#ifndef __ABL_MUTEX_MUTEX_H__
#define __ABL_MUTEX_MUTEX_H__

#include "abl/abl_export.h"

CPP_VISIBLE_BEGIN

typedef struct _mutex_s mutex_s;
/** 
 * Creates new mutex.
 * 
 * 
 * @return Pointer to the mutex or NULL if failed
 */
mutex_s* mutex_create (void);

/** 
 * Locks the mutex
 * 
 * @param mutex - mutex to lock
 */
void mutex_lock (mutex_s* mutex);

/** 
 * Unlocks the mutex
 * 
 * @param mutex - mutex to unlock
 */
void mutex_unlock (mutex_s* mutex);

/** 
 * Destroys the mutex
 * 
 * @param mutex - mutex to destroy
 */
void mutex_destroy (mutex_s* mutex);

CPP_VISIBLE_END


#endif
