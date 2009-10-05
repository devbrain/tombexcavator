#include "abl/mt/mutex.hpp"

namespace abl 
{
  mutex_c::mutex_c () TE_THROW_SPEC (std::runtime_error)
  {
    m_mutex = mutex_create ();
    if (!m_mutex)
      {
	TE_THROW (std::runtime_error ("Failed to create mutex"));
      }
  }
  // ------------------------------------------------------------------
  mutex_c::~mutex_c () TE_NOTHROW_SPEC
  {
    mutex_destroy (m_mutex);
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
}
