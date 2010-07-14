/**
 * @file   rw_lock.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 11:14:15 2009
 * 
 * @brief  readers writers lock facade
 * 
 * 
 */
#ifndef __ABL_MT_RW_LOCK_HPP__
#define __ABL_MT_RW_LOCK_HPP__

#include "abl/abl_export.h"
#include "abl/throw_spec.hpp" 
#include "abl/exception.hpp"
#include "abl/util/non_copyable.hpp" 
#include "abl/platform/mutex/rwlock.h"
namespace abl 
{
  struct rw_lock_impl_s;

  /**
   * @class rw_lock_c
   * @brief A reader writer lock allows multiple concurrent
   * readers or one exclusive writer.
   */
  class ABL_API rw_lock_c : public util::non_copyable_c 
  {
  public:
    /** 
     * Creates the Reader/Writer lock. 
     */
    rw_lock_c () TE_THROW_SPEC (system_exception_c);
    /** 
     * Destroys the Reader/Writer lock.
     */
    ~rw_lock_c () TE_NOTHROW_SPEC;
    /** 
     * Acquires a read lock. If another thread currently holds a write lock,
     * waits until the write lock is released.
     */
    void read_lock ();
    
    /**
     * Tries to acquire a read lock. Immediately returns true if successful, or
     * false if another thread currently holds a write lock.
     */
    bool try_read_lock ();
    
    /** 
     * Acquires a write lock. If one or more other threads currently hold 
     * locks, waits until all locks are released. The results are undefined
     * if the same thread already holds a read or write lock
     */
    void write_lock ();
  
    /** 
     * Tries to acquire a write lock. Immediately returns true if successful,
     * or false if one or more other threads currently hold 
     * locks. The result is undefined if the same thread already
     * holds a read or write lock.
     */
    bool try_write_lock ();
  
    /** 
     * Releases the read or write lock.
     */
    void unlock();
  
  private:
    rwlock_s* m_lock;
  };
} 

#endif 
