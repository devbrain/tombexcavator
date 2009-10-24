/*
 * owned_ptr - ownership aware auto-deleting pointer
 *
 * Note: Do not use in an STL container!
 */

#ifndef __ABL_PTR_OWNED_PTR_HPP__
#define __ABL_PTR_OWNED_PTR_HPP__

#include "abl/platform.h"
#include "abl/abl_export.h"
#include "abl/throw_spec.hpp"

#include "abl/ptr/destructor.hpp"

namespace abl
{
  template <class X> 
  class INLINE_API owned_ptr_c
  {
  public:
    typedef X element_type;
  public:
    // --------------------------------------------------------------------------------------
    /** 
     * @brief Constructor. 
     * 
     * Takes ownership of p if p is not null
     * @param p - pointer to take ownership
     * 
     */
    __te_explicit__ owned_ptr_c (X* p = 0) TE_NOTHROW_SPEC
    : m_is_owner (p != 0), 
      m_ptr      (p) 
    {
    }
    /** 
     * @brief Destructor.
     *
     *
     * Deletes underlying pointer if it is in my ownership
     * 
     */
    ~owned_ptr_c ()                            
    {
      if (m_is_owner) 
	{
	  _destroy ();
	}
    }
    /** 
     * @brief Copy constructor
     * 
     * Takes ownership of the argument. Argument will be released
     * @param r - owned_ptr to copy
     * 
     */
    owned_ptr_c (const owned_ptr_c>& r) TE_NOTHROW_SPEC
      : m_is_owner (r.m_is_owner), 
	m_ptr      (r.release ()) 
    {
    }
    /** 
     * @brief Assigment operator
     * 
     * Takes ownership of the argument. Argument will be released
     *
     */
    owned_ptr_c& operator=(const owned_ptr_c& r) TE_NOTHROW_SPEC
    {
      if (&r != this) 
	{
	  if (m_ptr != r.m_ptr) 
	    {
	      if (m_is_owner) 
		{
		  _destroy ();
		}
	      m_is_owner = r.m_is_owner;
	    }
	  else 
	    {
	      if (r.m_is_owner) 
		{
		  m_is_owner = true;
		}
	    }
	m_ptr = r.release();
      }
      return *this;
    }

#ifndef TE_NO_MEMBER_TEMPLATES
    template <class Y> friend class owned_ptr_c<Y>;
    template <class Y> owned_ptr_c (const owned_ptr_c <Y>& r) TE_NOTHROW_SPEC
    : m_is_owner (r.m_is_owner), 
      m_ptr      (r.release ()) 
    {
    }
    
    template <class Y> owned_ptr_c& operator = (const owned_ptr_c <Y>& r) TE_NOTHROW_SPEC
    {
      if (&r != this) 
	{
	  if (m_ptr != r.m_ptr) 
	    {
	      if (m_is_owner) 
		{
		  _destroy ();
		}
	      m_is_owner = r.m_is_owner;
	    }
	  else 
	    {
	      if (r.m_is_owner) 
		{
		  m_is_owner = true;
		}
	    }
	m_ptr = r.release();
      }
      return *this;
    }
#endif
    /** 
     * @brief dereference operator
     * 
     * 
     */
    X& operator*()  const TE_NOTHROW_SPEC 
    {
      return *m_ptr;
    }
    /** 
     * @brief access operator
     * 
     * 
     */
    X* operator->() const TE_NOTHROW_SPEC           
    {
      return m_ptr;
    }
    /** 
     * @brief obtain internal pointer
     * 
     * 
     * @return internal pointer
     */
    X* get() const TE_NOTHROW_SPEC           
    {
      return m_ptr;
    }
    /** 
     * @brief release my ownership from the pointer
     * 
     * 
     * @return internal pointer
     */
    X* release() const TE_NOTHROW_SPEC
    {
#if defined (TE_HAS_MUTABLE)
      m_is_owner = false; 
#else // for ANSI-challenged compilers
      (const_cast <owned_ptr_c<X>* >(this))->m_is_owner = false; 
#endif
      return m_ptr;
    }
  private:
    void _destroy ()
    {
      destructor_c <X> deleter (m_ptr);
      deleter ();
    }
  private:
    __te_mutable__ bool m_is_owner;
    X* m_ptr;
  };
} // ns abl


#endif
