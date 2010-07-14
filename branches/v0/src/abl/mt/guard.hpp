#ifndef __ABL_MT_GUARD_HPP__
#define __ABL_MT_GUARD_HPP__

#include "abl/abl_export.h"
#include "abl/util/non_copyable.hpp"

#define ABL_DECLARE_GUARD_CLASS(API, NAME, ACTION)	\
  template <class Lock>					\
  class API NAME : public util::non_copyable_c		\
  {							\
  public:						\
    typedef Lock lock_t;				\
  public:						\
    NAME (lock_t& lock)					\
      : m_lock (lock)					\
      {							\
	m_lock. ACTION ();				\
      }							\
      ~ NAME ()						\
	{						\
	  m_lock.unlock ();				\
	}						\
  private:						\
      lock_t& m_lock;					\
  }


namespace abl
{
  ABL_DECLARE_GUARD_CLASS (INLINE_API, guard_c        , lock);
  ABL_DECLARE_GUARD_CLASS (INLINE_API, readers_guard_c, read_lock);
  ABL_DECLARE_GUARD_CLASS (INLINE_API, writers_guard_c, write_lock);
} // ns abl


#endif
