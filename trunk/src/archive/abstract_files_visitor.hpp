#ifndef __ARCHIVE_ABSTRACT_FILES_VISITOR_HPP__
#define __ARCHIVE_ABSTRACT_FILES_VISITOR_HPP__

#include <string>

#include "platform/types.h"
#include "archive/archive_export.h"

namespace archive
{
  class abstract_archive_c;

  class ARCHIVE_API abstract_files_visitor_c
  {
  public:
    virtual ~abstract_files_visitor_c ();
    virtual void handle (const abstract_archive_c* ar, 
			 const std::string& fname) = 0;
  };
} // ns archive

#endif
