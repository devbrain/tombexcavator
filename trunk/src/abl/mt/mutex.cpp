#include "abl/mt/mutex.hpp"

namespace abl 
{
  mutex_c::mutex_c () TE_THROW_SPEC (system_exception_c)
  {
    m_mutex = mutex_create ();
    if (!m_mutex)
      {
	TE_THROW (system_exception_c ("Failed to create mutex"));
      }
  }
  // ------------------------------------------------------------------
  mutex_c::~mutex_c () TE_NOTHROW_SPEC
  {
    TE_DISABLE_EXCEPTION (mutex_destroy (m_mutex);)
  }
  // ------------------------------------------------------------------
  void mutex_c::lock ()
  {
    mutex_lock (m_mutex);
  }
  // ------------------------------------------------------------------
  void mutex_c::unlock ()
  {
    mutex_unlock (m_mutex);
  }
  // ------------------------------------------------------------------
  bool mutex_c::try_lock ()
  {
    return mutex_try_lock (m_mutex) == TE_TRUE;
  }
  // ------------------------------------------------------------------
  bool mutex_c::try_lock (int ms)
  {
    return mutex_try_lock_timeout (m_mutex, ms) == TE_TRUE;
  }
  // ------------------------------------------------------------------
  void mutex_c::read_lock ()
  {
    this->lock ();
  }
  // ------------------------------------------------------------------
  void mutex_c::write_lock ()
  {
    this->lock ();
  }
}
