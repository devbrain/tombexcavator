#include "abl/mt/null_mutex.hpp"

namespace abl 
{
  null_mutex_c::null_mutex_c () TE_THROW_SPEC (std::runtime_error)
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
} // ns abl
