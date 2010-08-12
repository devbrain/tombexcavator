#ifndef __PLATFORM_OS_H__
#define __PLATFORM_OS_H__

#include "platform/types.h"
#include "platform/platform_export.h"
#include "platform/os_error.h"

/**
 * File System API
 */
struct os_file_descriptor_s_;
typedef struct os_file_descriptor_s_ os_file_descriptor_s;

/** 
 * Opens file for read
 * 
 * @param path utf8 encoded file name
 * @param error will be set if error occured
 * 
 * @return pointer to opaque ds or NULL in case of error
 */
PLATFORM_API 
os_file_descriptor_s* os_open_file_for_read CALLSPEC (const char* path,
						      os_error_s** error);
/** 
 * Creates file. If file exists, it will be truncated.
 * 
 * @param path utf8 encoded file name
 * @param error will be set if error occured
 * 
 * @return pointer to opaque ds or NULL in case of error
 */
PLATFORM_API 
os_file_descriptor_s* os_create_file CALLSPEC (const char* path,
					       os_error_s** error);
/** 
 * Opens file in read/write mode.
 * 
 * @param path utf8 encoded file name
 * @param error will be set if error occured
 * 
 * @return pointer to opaque ds or NULL in case of error
 */
PLATFORM_API 
os_file_descriptor_s* os_open_file_for_append CALLSPEC (const char* path,
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
PLATFORM_API 
int os_flush_file CALLSPEC (os_file_descriptor_s* fd,
			    os_error_s** error);

/** 
 * closes file pointed by opaque ds, and deallocates this ds
 * 
 * @param fd pointer to opaque ds
 * @param error will be set if error occured
 * 
 * @return zero on failure. 
 */
PLATFORM_API 
int os_close_file CALLSPEC (os_file_descriptor_s* fd,
			    os_error_s** error);

/** 
 * writes data to file
 * 
 * @param fd opaque ds
 * @param buff buffer to write
 * @param size size of buffer
 * @param written how many bytes were written
 * @param error will be set if error occured
 * 
 * @return zero on failure
 */
PLATFORM_API 
int os_write_to_file CALLSPEC (os_file_descriptor_s* fd,
			       const void* buff,
			       size_t      size,
			       size_t*     written,
			       os_error_s** error);
/** 
 * Reads data from file
 * 
 * @param fd opaque ds
 * @param buff buffer to read data to
 * @param size size of the buffer
 * @param has_bytes how many bytes were red to the buffer
 * @param error will be set if error occured
 * 
 * @return zero on failure
 */
PLATFORM_API 
int os_read_from_file CALLSPEC (os_file_descriptor_s* fd,
				void*   buff,
				size_t  size,
				size_t* has_bytes,
				os_error_s** error);

typedef uint64_t os_file_offset_t;

/** 
 * returns the size of the file
 * 
 * @param fd opaque ds
 * @param error will be set if error occured
 * 
 * @return size of the file in bytes
 */
PLATFORM_API 
os_file_offset_t os_file_size CALLSPEC (os_file_descriptor_s* fd,
					os_error_s** error);

/** 
 * returns the current position in the file
 * 
 * @param fd opaque ds
 * @param error will be set if error occured
 * 
 * @return current position in file
 */
PLATFORM_API 
os_file_offset_t os_current_file_position CALLSPEC (os_file_descriptor_s* fd,
						    os_error_s** error);

enum os_seek_direction_t_
  {
    eFROM_START,		/**< seek from start                    */
    eFROM_END,			/**< seek from end                      */
    eFROM_CURRENT		/**< seek from current position in file */
  };

typedef enum os_seek_direction_t_ os_seek_direction_t;
/** 
 * seeks file
 * 
 * @param fd opaque ds
 * @param offset offset to seek
 * @param direction whence
 *
 * @return zero on failure
 */
PLATFORM_API 
int os_set_current_file_position CALLSPEC (os_file_descriptor_s* fd,
					   os_file_offset_t      offset,
					   os_seek_direction_t   direction);
/** 
 * query if the given path exists
 * 
 * @param path path to query
 * 
 * @return zero if path does not exists
 */
PLATFORM_API 
int os_path_exists CALLSPEC (const char* path);

/** 
 * query if path is a link (hard/symb.)
 * 
 * @param path utf8 encoded path
 * 
 * @return nonzero if path is a link
 */
PLATFORM_API 
int os_path_is_link CALLSPEC (const char* path);

/** 
 * query if path is an ordinal file
 * 
 * @param path utf8 encoded path
 * 
 * @return nonzero if path is an ordinal file 
 */
PLATFORM_API 
int os_path_is_file CALLSPEC (const char* path);

/** 
 * query if path is directory
 * 
 * @param path utf8 encoded path
 * 
 * @return nonzero if path is a directory
 */
PLATFORM_API 
int os_path_is_directory CALLSPEC (const char* path);

/**
 * @typedef enum_files_callback_t
 * @brief Function signature for callbacks that enumerate files.
 *
 * These are used to report a list of directory entries to an original caller,
 *  one file/dir/symlink per callback. All strings are UTF-8 encoded.
 *  Functions should not try to modify or free any string's memory.
 *
 * These callbacks are used, starting in PhysicsFS 1.1, as an alternative to
 *  functions that would return lists that need to be cleaned up with
 *  PHYSFS_freeList(). The callback means that the library doesn't need to
 *  allocate an entire list and all the strings up front.
 *
 * Be aware that promises data ordering in the list versions are not
 *  necessarily so in the callback versions. Check the documentation on
 *  specific APIs, but strings may not be sorted as you expect.
 *
 *    @param data User-defined data pointer, passed through from the API
 *                that eventually called the callback.
 *    @param origdir A string containing the full path, in platform-independent
 *                   notation, of the directory containing this file. In most
 *                   cases, this is the directory on which you requested
 *                   enumeration, passed in the callback for your convenience.
 *    @param fname The filename that is being enumerated. It may not be in
 *                 alphabetical order compared to other callbacks that have
 *                 fired, and it will not contain the full path. You can
 *                 recreate the fullpath with $origdir/$fname ... The file
 *                 can be a subdirectory, a file, a symlink, etc.
 *
 * @sa enum_files_callback_t
 */
typedef void (*enum_files_callback_t)(void *data, const char *origdir,
				      const char *fname);

PLATFORM_API 
void os_enumerate_files CALLSPEC (const char *dirname,
				  int omit_sym_links,
				  enum_files_callback_t callback,
				  const char *origdir,
				  void *callbackdata);
/**
 * Mutex API
 */

struct os_mutex_s_;
typedef struct os_mutex_s_ os_mutex_s;

PLATFORM_API 
os_mutex_s* os_mutex_create CALLSPEC (void);

PLATFORM_API 
void  os_mutex_lock CALLSPEC (os_mutex_s* mutex);

PLATFORM_API 
void os_mutex_unlock CALLSPEC (os_mutex_s* mutex);

PLATFORM_API 
void os_mutex_destroy CALLSPEC (os_mutex_s* mutex);


/**
 * Shared Objects API
 */

struct os_shared_object_s_;
typedef struct os_shared_object_s_ os_shared_object_s;

PLATFORM_API 
os_shared_object_s* os_shared_object_open CALLSPEC (const char* path);

PLATFORM_API 
void* os_shared_object_get_symbol CALLSPEC (os_shared_object_s* so, 
					    const char* name);
PLATFORM_API 
void os_shared_object_close CALLSPEC (os_shared_object_s* so);


#endif
