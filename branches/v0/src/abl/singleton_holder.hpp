#ifndef __ABL_SINGLETON_HOLDER_HPP__
#define __ABL_SINGLETON_HOLDER_HPP__

#include "abl/abl_export.h"
#include "abl/mt/guard.hpp"

namespace abl
{
  /**
   * @class singleton_holder_c
   *
   * This is a helper template class for managing
   * singleton objects allocated on the heap.
   * The class ensures proper deletion (including
   * calling of the destructor) of singleton objects
   * when the application that created them terminates.
   */
  template <class S, class Mutex>
  class INLINE_API singleton_holder_c
  {
  public:
    typedef S     value_t;
    typedef Mutex mutex_t;
  public:
    /** 
     * Creates the singleton_holder_c.
     */
    singleton_holder_c ()
    {
      m_obj = 0;
    }
    /**
     * Destroys the singleton_holder_c and the singleton
     * object that it holds.
     */
    ~singleton_holder_c()
    {
      delete m_obj;
    }
    /** 
     * Returns a pointer to the singleton object
     * hold by the singleton_holder_c. The first call
     * to get will create the singleton.
     */
    S* get()
    {
      guard_c <Mutex> lock (m_mtx);
      if (!m_obj) 
	{
	  m_obj = new S;
	}
      return m_obj;
    }
  private:
    S*    m_obj;
    Mutex m_mtx;
  };
 
}


#endif
