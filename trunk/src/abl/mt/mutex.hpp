#ifndef __ABL_MT_MUTEX_HPP__
#define __ABL_MT_MUTEX_HPP__

#include "abl/abl_export.h"
#include "abl/throw_spec.hpp"

#include "abl/platform/mutex/mutex.h"


namespace abl
{
  class ABL_API mutex_c
  {
  public:
    mutex_c     () TE_THROW_SPEC (std::runtime_error);
    ~mutex_c    () TE_NOTHROW_SPEC;
    void lock   ();
    void unlock ();
  private:
    mutex_s* m_mutex;
  };
}// ns abl


#endif
