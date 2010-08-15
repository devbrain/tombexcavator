#ifndef __C_ARCHIVE_ADAPTOR_HPP__
#define __C_ARCHIVE_ADAPTOR_HPP__


#include "archive/abstract_archive.hpp"

#if defined(__cplusplus)
extern "C" {
#endif

#include "archive/archive.h"

#if defined(__cplusplus)
}
#endif

namespace archive
{
  ARCHIVE_API archive_c* c_archive_adaptor (archive_c* ar);
}


#endif

