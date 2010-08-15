#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include "platform/types.h"
#include "archive/archive_export.h"


typedef int (*is_archive_t) (const char* path);
typedef void* (open_archive_t) (const char* path);
typedef void  (close_archive_t) (void* archive);
typedef void (*archive_enum_files_callback_t)(void *data, 
					      const char *fname);
typedef void (*archive_enum_files_t)(void *opaque,
				     const char *dirname,
				     int omitSymLinks,
				     archive_enum_files_callback_t callback,
				     void *callbackdata);
typedef  int (*file_exists_in_archive_t )(void *opaque, 
					  const char *name);
typedef int (*is_dir_in_archive_t)(void* opaque, 
				   const char* name, 
				   int* fileExists);
typedef int (*is_symlink_in_archive_t) (void *opaque, 
					const char *name, 
					int *fileExists);
typedef int (*is_file_in_archive_t) (void *opaque, 
				     const char *name, 
				     int *fileExists);

enum archive_file_mode_t_
  {
    eOPEN_READ,
    eCREATE,
    eOPEN_READ_WRITE
  };

typedef archive_file_mode_t_ archive_file_mode_t;
typedef void* (*open_file_in_archive_t) (void *opaque, 
					 const char *fname, 
					 int *fileExists,
					 archive_file_mode_t mode);


typedef int (*remove_file_in_archive_t) (void *opaque, 
					 const char *filename);
typedef int (*mkdir_in_archive_t) (void *opaque, 
				   const char *filename);

typedef int64_t (*file_read_t)(void *fopaque, 
				void *buffer,
				size_t objSize);

typedef int64_t (*file_write_t)(void *fopaque, 
			       const void *buffer,
			       size_t objSize);

typedef int64_t (*file_tell_t) (void *fopaque);

typedef int (*file_seek_t)(void *fopaque, uint64 offset);

typedef int64_t (*file_length_t)(void *fopaque);

typedef int (*file_close_t)(void *fopaque);

typedef int (*file_sync_t)(void *fopaque);

struct ARCHIVE_API archive_s_
{
  is_archive_t             is_archive;
  open_archive_t           open_archive;
  close_archive_t          close_archive;
  archive_enum_files_t     enum_files; 
  file_exists_in_archive_t exists;
  is_dir_in_archive_t      is_dir;
  is_symlink_in_archive_t  is_symlink;
  is_file                  is_file;
  open_file_in_archive_t   open_file;
  remove_file_in_archive_t remove_file;
  mkdir_in_archive_t       mkdir;
  file_read_t              file_read;
  file_write_t             file_write;
  file_tell_t              file_tell;
  file_seek_t              file_seek;
  file_length_t            file_length;
  file_close_t             file_close;
  file_sync_t              file_sync;
};

typedef archive_s_ archive_s;

#endif
