#ifndef __UTILS_BLOCK_GUARD_HPP__
#define __UTILS_BLOCK_GUARD_HPP__

#include "utils/guard/destructor.hpp"

namespace utils
{
  template <typename T>
  class block_guard_c
  {
  public:
    block_guard_c (T* ptr);
    ~block_guard_c ();
    T* operator -> ();
    const T* operator -> () const;
    operator bool () const;
    operator T* ();
    operator const T* () const;
    void release ();
  private:
    T* m_ptr;
  };

  template <typename T>
  inline
  block_guard_c<T> make_block_guard (T* ptr)
  {
    return block_guard_c<T> (ptr);
  }
} // ns utils
// ===============================================================
// Implementation
// ===============================================================
namespace utils
{
  template <typename T>
  inline
  block_guard_c<T>::block_guard_c (T* ptr)
    : m_ptr (ptr)
  {
        
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  block_guard_c<T>::~block_guard_c ()
  {
    destructor_c <T> d(m_ptr);
    d ();
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  T* block_guard_c<T>::operator -> ()
  {
    return m_ptr;
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  const T* block_guard_c<T>::operator -> () const
  {
    return m_ptr;
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  block_guard_c<T>::operator bool () const
  {
    return true;
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  block_guard_c<T>::operator T* ()
  {
    return m_ptr;
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  block_guard_c<T>::operator const T* () const
  {
    return m_ptr;
  }
  // -----------------------------------------------------------
  template <typename T>
  inline
  void block_guard_c<T>::release () 
  {
    m_ptr = 0;
  }
  
}

#endif
