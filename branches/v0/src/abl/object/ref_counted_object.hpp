#ifndef __ABL_OBJECT_REF_COUNTED_OBJECT_HPP__
#define __ABL_OBJECT_REF_COUNTED_OBJECT_HPP__


#include "abl/abl_export.h"
#include "abl/mt/atomic_counter.hpp"
#include "abl/mt/fast_mutex.hpp"

namespace abl 
{
  /**
   * A base class for objects that employ
   * reference counting based garbage collection.
   *
   * Reference-counted objects inhibit construction
   * by copying and assignment.
   */
  class ABL_API ref_counted_object_c
	
  {
  public:
    /**
     * Creates the ref_counted_object_c.
     * The initial reference count is one.
     */
    ref_counted_object_c ();
    /**
     * Increments the object's reference count.
     */
    void duplicate () const;
    /**
     * Decrements the object's reference count
     * and deletes the object if the count
     * reaches zero.
     */
    void release () const;
    /**
     * Returns the reference count.
     */
    int reference_count () const;
  protected:
    /**
     * Destroys the ref_counted_object_c.
     */
    virtual ~ref_counted_object_c ();
  private:
    ref_counted_object_c (const ref_counted_object_c&);
    ref_counted_object_c& operator =  (const ref_counted_object_c&);

    mutable atomic_counter_c <fast_mutex_c, int> m_counter;
  };
} 


#endif 
