#include "abl/mt/null_mutex.hpp"

namespace abl 
{
  null_mutex_c::null_mutex_c () TE_THROW_SPEC (system_exception_c)
  {
  }
  // ------------------------------------------------------------------
  null_mutex_c::~null_mutex_c () TE_NOTHROW_SPEC
  {
  }
  // ------------------------------------------------------------------
  void null_mutex_c::lock ()
  {
  }
  // ------------------------------------------------------------------
  void null_mutex_c::unlock ()
  {
  }
  // ------------------------------------------------------------------
  bool null_mutex_c::try_lock ()
  {
    return true;
  }
  // ------------------------------------------------------------------
  bool null_mutex_c::try_lock (int ms)
  {
    return true;
  }
  // ------------------------------------------------------------------
  void null_mutex_c::read_lock ()
  {
  }
  // ------------------------------------------------------------------
  void null_mutex_c::write_lock ()
  {
  }
} // ns abl
