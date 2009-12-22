#ifndef __ABL_MT_MUTEX_HPP__
#define __ABL_MT_MUTEX_HPP__

#include "abl/abl_export.h"
#include "abl/throw_spec.hpp"
#include "abl/exception.hpp"

namespace abl
{
  class ABL_API null_mutex_c
  {
  public:
    null_mutex_c     () TE_THROW_SPEC (system_exception_c);
    ~null_mutex_c    () TE_NOTHROW_SPEC;
    void lock   ();
    void unlock ();
    bool try_lock ();
    bool try_lock (int ms);
    void read_lock ();
    void write_lock ();
  };
}// ns abl

#endif

