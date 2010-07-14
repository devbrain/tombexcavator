#ifndef __ABL_MT_MUTEX_HPP__
#define __ABL_MT_MUTEX_HPP__

#include "abl/abl_export.h"
#include "abl/throw_spec.hpp"
#include "abl/exception.hpp"
#include "abl/platform/mutex/mutex.h"


namespace abl
{
  class ABL_API mutex_c
  {
  public:
    mutex_c     () TE_THROW_SPEC (system_exception_c);
    ~mutex_c    () TE_NOTHROW_SPEC;
    void lock   ();
    void unlock ();
    bool try_lock ();
    bool try_lock (int ms);
    void read_lock ();
    void write_lock ();
  private:
    mutex_s* m_mutex;
  };
}// ns abl


#endif
