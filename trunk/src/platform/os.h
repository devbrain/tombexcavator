#ifndef __PLATFORM_OS_H__
#define __PLATFORM_OS_H__

#include "platform/types.h"
#include "platform/platform_export.h"
#include "platform/os_error.h"

/**
 * File System API
 */
struct file_descriptor_s_;
typedef struct file_descriptor_s_ file_descriptor_s;

/** 
 * Opens file for read
 * 
 * @param path utf8 encoded file name
 * @param error will be set if error occured
 * 
 * @return pointer to opaque ds or NULL in case of error
 */
file_descriptor_s* open_file_for_read   (const char* path,
					 os_error_s** error);
/** 
 * Creates file. If file exists, it will be truncated.
 * 
 * @param path utf8 encoded file name
 * @param error will be set if error occured
 * 
 * @return pointer to opaque ds or NULL in case of error
 */
file_descriptor_s* create_file          (const char* path,
					 os_error_s** error);
/** 
 * Opens file in read/write mode.
 * 
 * @param path utf8 encoded file name
 * @param error will be set if error occured
 * 
 * @return pointer to opaque ds or NULL in case of error
 */
file_descriptor_s* open_file_for_append (const char* path,
					 os_error_s** error);
/** 
 *  For  output streams, forces a write of all user-space buffered
 *  data for the given output or update stream via the stream's  underlying
 *  write function.  For input streams, discards any buffered data
 *  that has been fetched from the underlying file, but has not been by the
 *  application.  The open status of the stream is unaffected.
 * 
 * @param fd pointer to opaque ds
 * @param error will be set if error occured
 * 
 * @return zero on failure
 */

int flush_file (file_descriptor_s* fd,
		os_error_s** error);

/** 
 * closes file pointed by opaque ds, and deallocates this ds
 * 
 * @param fd pointer to opaque ds
 * @param error will be set if error occured
 * 
 * @return zero on failure. 
 */
int close_file (file_descriptor_s* fd,
		os_error_s** error);

/** 
 * writes data to file
 * 
 * @param fd opaque ds
 * @param buff buffer to write
 * @param size size of buffer
 * @param count number of times to write the buffer
 * @param written how many bytes were written
 * @param error will be set if error occured
 * 
 * @return zero on failure
 */

int write_to_file (file_descriptor_s* fd,
		   const void* buff,
		   size_t      size,
		   size_t      count,
		   size_t*     written,
		   os_error_s** error);
/** 
 * Reads data from file
 * 
 * @param fd opaque ds
 * @param buff buffer to read data to
 * @param size size of the buffer
 * @param count how many times to read data
 * @param has_bytes how many bytes were red to the buffer
 * @param error will be set if error occured
 * 
 * @return zero on failure
 */
int read_from_file (file_descriptor_s* fd,
		    void*   buff,
		    size_t  size,
		    size_t  count,
		    size_t* has_bytes,
		    os_error_s** error);

typedef uint64_t file_offset_t;

/** 
 * returns the size of the file
 * 
 * @param fd opaque ds
 * @param error will be set if error occured
 * 
 * @return size of the file in bytes
 */
file_offset_t file_size (file_descriptor_s* fd,
			 os_error_s** error);

/** 
 * returns the current position in the file
 * 
 * @param fd opaque ds
 * @param error will be set if error occured
 * 
 * @return current position in file
 */
file_offset_t current_file_position (file_descriptor_s* fd,
				     os_error_s** error);

enum seek_direction_t_
  {
    eFROM_START,		/**< seek from start */
    eFROM_END,			/**< seek from end */
    eFROM_CURRENT		/**< seek from current position in file */
  };

typedef enum seek_direction_t_ seek_direction_t;
/** 
 * seeks file
 * 
 * @param fd opaque ds
 * @param offset offset to seek
 * @param direction whence
 *
 * @return zero on failure
 */
int set_current_file_position (file_descriptor_s* fd,
				file_offset_t      offset,
				seek_direction_t   direction);
/** 
 * query if path is a link (hard/symb.)
 * 
 * @param path utf8 encoded path
 * 
 * @return nonzero if path is a link
 */
int path_is_link      (const char* path);

/** 
 * query if path is an ordinal file
 * 
 * @param path utf8 encoded path
 * 
 * @return nonzero if path is an ordinal file 
 */
int path_is_file      (const char* path);

/** 
 * query if path is directory
 * 
 * @param path utf8 encoded path
 * 
 * @return nonzero if path is a directory
 */
int path_is_directory (const char* path);

/**
 * Mutex API
 */

struct mutex_s_;
typedef struct mutex_s_ mutex_s;

mutex_s* mutex_create (void);
void mutex_lock (mutex_s* mutex);
int mutex_try_lock (mutex_s* mutex);
int mutex_try_lock_timeout (mutex_s* mutex, int msecs);
void mutex_unlock (mutex_s* mutex);
void mutex_destroy (mutex_s* mutex);

struct rwlock_s_;
typedef struct rwlock_s_ rwlock_s;

rwlock_s* rwlock_create         (void);
void      rwlock_read_lock      (rwlock_s* rwlock);
int       rwlock_read_try_lock  (rwlock_s* rwlock);
void      rwlock_write_lock     (rwlock_s* rwlock);
int       rwlock_write_try_lock (rwlock_s* rwlock);
void      rwlock_unlock         (rwlock_s* rwlock);  
void      rwlock_destroy        (rwlock_s* rwlock);

/**
 * Shared Objects API
 */

struct shared_object_s_;
typedef struct shared_object_s_ shared_object_s;

shared_object_s* shared_object_open (const char* path);
void* shared_object_get_symbol (shared_object_s* so, 
				const char* name);
void shared_object_close (shared_object_s* so);


#endif
