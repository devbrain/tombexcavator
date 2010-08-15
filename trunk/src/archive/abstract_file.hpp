#ifndef __ARCHIVE_ABSTRACT_FILE_HPP__
#define __ARCHIVE_ABSTRACT_FILE_HPP__

#include "platform/types.h"
#include "archive/archive_export.h"

namespace archive
{
  class ARCHIVE_API abstract_file_c
  {
  public:
    virtual ~abstract_file_c ();
    virtual int64_t read (void* buff, size_t size) = 0;
    virtual int64_t write (const void* buff, size_t size) = 0;
    virtual int64_t tell  () const = 0;
    virtual bool    seek (uint64_t offset) = 0;
    virtual bool    sync () = 0;
  };
} // ns archive

#endif
