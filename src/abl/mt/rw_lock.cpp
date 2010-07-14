#include "abl/mt/rw_lock.hpp"

namespace abl 
{

  rw_lock_c::rw_lock_c () TE_THROW_SPEC (system_exception_c)
  {
    m_lock = rwlock_create ();
    if (!m_lock)
      {
	TE_THROW (system_exception_c ("cannot create reader/writer lock"));
      }
  }
  // ---------------------------------------------------------------------
  rw_lock_c::~rw_lock_c () TE_NOTHROW_SPEC
  {
    TE_DISABLE_EXCEPTION (rwlock_destroy (m_lock);)
  }
  // ---------------------------------------------------------------------
  void rw_lock_c::read_lock ()
  {
    rwlock_read_lock (m_lock);
  }
  // ---------------------------------------------------------------------
  bool rw_lock_c::try_read_lock ()
  {
    return (rwlock_read_try_lock (m_lock) == TE_TRUE);
  }
  // ---------------------------------------------------------------------
  void rw_lock_c::write_lock ()
  {
    rwlock_write_lock (m_lock);
  }
  // ---------------------------------------------------------------------
  bool rw_lock_c::try_write_lock ()
  {
    return (rwlock_write_try_lock (m_lock) == TE_TRUE);
  }
  // ---------------------------------------------------------------------
  void rw_lock_c::unlock()
  {
    rwlock_unlock (m_lock);
  }
} // ns abl
