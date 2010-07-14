/**
 * @file   buffer.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Dec 27 09:19:49 2009
 * 
 * @brief  buffer_c
 * 
 */

#ifndef __ABL_BUFFER_HPP__
#define __ABL_BUFFER_HPP__


#include "abl/abl_export.h"
#include <cstddef>
#include <assert.h>

namespace abl 
{
  /**
   * A very simple buffer class that allocates a buffer of
   * a given type and size in the constructor and
   * deallocates the buffer in the destructor.
   *
   * This class is useful everywhere where a temporary buffer
   * is needed.
   */
  template <class T>
  class buffer_c
	
  {
  public:
    /**
     * Creates and allocates the buffer_c.
     */
    buffer_c (std::size_t size):
      m_size (size),
      m_ptr (new T[size])
		
    {
    }
    /**
     * Destroys the buffer_c.	
     */
    ~buffer_c ()

    {
      delete [] m_ptr;
    }
    /**
     * Returns the size of the buffer.	
     */
    std::size_t size () const

    {
      return m_size;
    }
    /**
     * Returns a pointer to the beginning of the buffer.	
     */
    T* begin ()

    {
      return m_ptr;
    }
    /**
     * Returns a pointer to the beginning of the buffer.	
     */
    const T* begin () const

    {
      return m_ptr;
    }
    /**
     * Returns a pointer to end of the buffer.
     */
    T* end ()

    {
      return m_ptr + m_size;
    }
    /**
     * Returns a pointer to the end of the buffer.	
     */
    const T* end () const

    {
      return m_ptr + m_size;
    }
	
    T& operator []  (std::size_t index)
    {
      assert  (index < m_size);
		
      return m_ptr[index];
    }

    const T& operator []  (std::size_t index) const
    {
      assert  (index < m_size);
		
      return m_ptr[index];
    }

  private:
    buffer_c ();
    buffer_c (const buffer_c&);
    buffer_c& operator =  (const buffer_c&);
  private:
    std::size_t m_size;
    T*          m_ptr;
  };
} 


#endif 
