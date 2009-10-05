#ifndef __ABL_MT_GUARD_HPP__
#define __ABL_MT_GUARD_HPP__

#include "abl/abl_export.h"
#include "abl/util/non_copyable.hpp"

namespace abl
{
  template <class Lock>
  class INLINE_API guard_c : public util::non_copyable_c
  {
  public:
    typedef Lock lock_t;
  public:
    /** 
     * Acquire lock
     * 
     * @param lock - lock to acquire
     * 
     */
    guard_c (lock_t& lock)
      : m_lock (lock)
    {
      m_lock.lock ();
    }
    /** 
     * release underlying lock
     * 
     * 
     */
    ~guard_c ()
    {
      m_lock.release ();
    }
  private:
    lock_t& m_lock;
  };
} // ns abl


#endif
