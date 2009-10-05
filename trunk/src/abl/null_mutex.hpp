#ifndef __ABL_MUTEX_HPP__
#define __ABL_MUTEX_HPP__

#include "abl/abl_export.h"
#include "abl/throw_spec.hpp"

namespace abl
{
  class ABL_API null_mutex_c
  {
  public:
    null_mutex_c     () TE_THROW_SPEC (std::runtime_error);
    ~null_mutex_c    () TE_NOTHROW_SPEC;
    void lock   ();
    void unlock ();
  };
}// ns abl

#endif

