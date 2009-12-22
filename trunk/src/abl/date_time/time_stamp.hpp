/**
 * @file   time_stamp.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 15:25:44 2009
 * 
 * @brief  Definition of the time_stamp_c class.
 * 
 */

#ifndef __ABL_DATE_TIME_TIME_STAMP_HPP__
#define __ABL_DATE_TIME_TIME_STAMP_HPP__


#include "abl/abl_export.h"
#include "abl/util/types.hpp"

#include <ctime>


namespace abl 
{
  /**
   * A time_stamp_c stores a monotonic* time value
   * with (theoretical) microseconds resolution.
   * time_stamp_cs can be compared with each other
   * and simple arithmetics are supported.
   *
   * [*] Note that time_stamp_c values are only monotonic as
   * long as the systems's clock is monotonic as well
   * (and not, e.g. set back).
   *
   * time_stamp_cs are UTC (Coordinated Universal Time)
   * based and thus independent of the timezone
   * in effect on the system.
   */
  class ABL_API time_stamp_c
	
  {
  public:
    typedef types::sint64_t time_val_t;     /**< monotonic UTC time value in microsecond resolution */
    typedef types::sint64_t utc_time_val_t; /**< monotonic UTC time value in 100 nanosecond resolution */
    typedef types::sint64_t time_diff_t;    /**< difference between two timestamps in microseconds */
    /**
     * Creates a timestamp with the current time.
     */
    time_stamp_c();
    /**
     * Creates a timestamp from the given time value.
     */
    time_stamp_c(time_val_t tv);
  
  
    time_stamp_c(const time_stamp_c& other);
  
    ~time_stamp_c();
  
    time_stamp_c& operator = (const time_stamp_c& other);
    time_stamp_c& operator = (time_val_t tv);
    /**
     * Swaps the time_stamp_c with another one.
     */
    void swap (time_stamp_c& timestamp);
    /**
     * Updates the time_stamp_c with the current time.
     */
    void update ();
		
  
    bool operator == (const time_stamp_c& ts) const;
    bool operator != (const time_stamp_c& ts) const;
    bool operator >  (const time_stamp_c& ts) const;
    bool operator >= (const time_stamp_c& ts) const;
    bool operator <  (const time_stamp_c& ts) const;
    bool operator <= (const time_stamp_c& ts) const;
	
    time_stamp_c  operator +  (time_diff_t d) const;
    time_stamp_c  operator -  (time_diff_t d) const;
    time_diff_t   operator -  (const time_stamp_c& ts) const;
    time_stamp_c& operator += (time_diff_t d);
    time_stamp_c& operator -= (time_diff_t d);
    /**
     * Returns the timestamp expressed in time_t.
     * time_t base time is midnight, January 1, 1970.
     * Resolution is one second.
     */
    std::time_t epoch_time () const;
    /**
     * Returns the timestamp expressed in UTC-based
     * time. UTC base time is midnight, October 15, 1582.
     * Resolution is 100 nanoseconds.
     */
    utc_time_val_t utc_time () const;
    /**
     * Returns the timestamp expressed in microseconds
     * since the Unix epoch, midnight, January 1, 1970.
     */
    time_val_t epoch_microseconds () const;
    /**
     * Returns the time elapsed since the time denoted by
     * the timestamp. Equivalent to time_stamp_c() - *this.
     */
    time_diff_t elapsed () const;
    /**
     * Returns true iff the given interval has passed
     * since the time denoted by the timestamp.
     */
    bool is_elapsed (time_diff_t interval) const;
    /**
     * Creates a timestamp from a std::time_t.
     */
    static time_stamp_c from_epoch_time (std::time_t t);
    /**
     * Creates a timestamp from a UTC time value.
     */
    static time_stamp_c from_utc_time (utc_time_val_t val);
    /**
     * Returns the resolution in units per second.
     * Since the timestamp has microsecond resolution,
     * the returned value is always 1000000.
     */
    static time_val_t resolution ();
		

#if defined(_WIN32)
    static time_stamp_c from_file_time_np (types::uint32_t fileTimeLow, types::uint32_t fileTimeHigh);
    void to_file_time_np (types::uint32_t& fileTimeLow, types::uint32_t& fileTimeHigh) const;
#endif

  private:
    time_val_t m_ts;
  };

  void swap(time_stamp_c& s1, time_stamp_c& s2);
} 


#endif 
