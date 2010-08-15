#ifndef __ABSTRACT_ARCHIVE_HPP__
#define __ABSTRACT_ARCHIVE_HPP__

#include <string>

#include "platform/types.h"
#include "archive/archive_export.h"

namespace archive
{
  class abstract_files_visitor_c;
  class abstract_file_c;
  
  class ARCHIVE_API abstract_archive_c
  {
  public:
    virtual ~abstract_archive_c ();

    virtual bool is_read_only () const = 0;

    virtual void open (const char* path) = 0;
    virtual void enum_files (const char* path, 
			     bool omit_links, 
			     abstract_files_visitor_c* visitor) const = 0;

    virtual bool exists  (const std::string& path) const = 0;
    virtual bool is_file (const std::string& path) const = 0;
    virtual bool is_dir  (const std::string& path) const = 0;
    virtual bool is_link (const std::string& path) const = 0;

    virtual bool remove (const std::string& path) = 0;
    virtual bool mkdir  (const std::string& path) = 0;

    virtual abstract_file_c* open (const std::string& path, bool for_write) = 0;
    virtual abstract_file_c* create (const std::string& path) = 0;
  };
} // ns archive

#endif
