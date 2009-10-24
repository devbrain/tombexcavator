#ifndef __ABL_MT_ATOMIC_COUNTER_HPP__
#define __ABL_MT_ATOMIC_COUNTER_HPP__

#include "abl/platform.h"
#include "abl/abl_export.h"
#include "abl/util/non_copyable.hpp"

namespace abl
{
  template <class Lock, typename Value>
  class INLINE_API atomic_counter_c : public util::non_copyable_c
  {
  public:
    typedef Lock  lock_t;
    typedef Value value_t;
  public:
    atomic_counter_c () 
       : m_counter (0) 
    { 
    }
    // ------------------------------------------------------------
    __te_explicit__ atomic_counter_c (value_t v)
      : m_counter (v)
    {
    }
    // ------------------------------------------------------------
    value_t operator++ () 
    { 
      m_mutex.lock();
      value_t r = ++m_counter;
      m_mutex.unlock();
      return r;
    }
    // ------------------------------------------------------------
    value_t operator++ (int) 
    { 
      m_mutex.lock();
      value_t r = m_counter++;
      m_mutex.unlock();
      return r;
    }
    // ------------------------------------------------------------
    value_t operator-- () 
    { 
      m_mutex.lock();
      value_t r = --m_counter;
      m_mutex.unlock();
      return r;
    }
    // ------------------------------------------------------------
    value_t operator-- (int) 
    { 
      m_mutex.lock();
      value_t r = m_counter--;
      m_mutex.unlock();
      return r;
    }
    // ------------------------------------------------------------
    value_t value () const
    {
#if defined (TE_HAS_MUTABLE)
      m_mutex.lock();
#else
      (const_cast <atomic_counter_c <Value>* >(this))->m_mutex.lock ();
#endif
      value_t r = m_counter;
#if defined (TE_HAS_MUTABLE)
      m_mutex.unlock();
#else
      (const_cast <atomic_counter_c <Value>* >(this))->m_mutex.unlock ();
#endif
      return r;
    }
  private:
    __te_mutable__ Lock m_mutex;
    Value               m_counter;
  };
} // ns abl

#endif
