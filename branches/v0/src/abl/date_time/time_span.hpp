/**
 * @file   time_span.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 15:50:19 2009
 * 
 * @brief  Definition of the time_span_c class.
 */

#ifndef __ABL_DATE_TIME_TIME_SPAN_HPP__
#define __ABL_DATE_TIME_TIME_SPAN_HPP__

#include "abl/abl_export.h"
#include "abl/date_time/time_stamp.hpp"


namespace abl
{
  /**
   * A class that represents time spans up to microsecond resolution.
   */
  class ABL_API time_span_c
  {
  public:
    typedef time_stamp_c::time_diff_t time_diff_t;
    /**
    * Creates a zero time_span_c.
    */
    time_span_c();
    /**
    * Creates a time_span_c.
    */
    time_span_c (time_diff_t microseconds);
    /**
    * Creates a time_span_c. Useful for creating
    * a time_span_c from a struct timeval.
    */
    time_span_c (long seconds, long microseconds);
    /**
    * Creates a time_span_c.
    */
    time_span_c (int days, int hours, int minutes, int seconds, int microseconds);
    /**
    * Creates a time_span_c from another one.
    */
    time_span_c (const time_span_c& timespan);
    /**
    * Destroys the time_span_c.
    */
    ~time_span_c ();
    /**
    * Assignment operator.
    */
    time_span_c& operator = (const time_span_c& timespan);
    /**
    * Assignment operator.
    */
    time_span_c& operator = (time_diff_t microseconds);
    /**
    * Assigns a new span.
    */
    time_span_c& assign (int days, int hours, int minutes, int seconds, int microseconds);
    /**
    * Assigns a new span. Useful for assigning
    * from a struct timeval.
    */
    time_span_c& assign (long seconds, long microseconds);
    /**
    * Swaps the time_span_c with another one.
    */
    void swap (time_span_c& timespan);

    bool operator == (const time_span_c& ts) const;
    bool operator != (const time_span_c& ts) const;
    bool operator >  (const time_span_c& ts) const;
    bool operator >= (const time_span_c& ts) const;
    bool operator <  (const time_span_c& ts) const;
    bool operator <= (const time_span_c& ts) const;

    bool operator == (time_diff_t microseconds) const;
    bool operator != (time_diff_t microseconds) const;
    bool operator >  (time_diff_t microseconds) const;
    bool operator >= (time_diff_t microseconds) const;
    bool operator <  (time_diff_t microseconds) const;
    bool operator <= (time_diff_t microseconds) const;
	
    time_span_c operator + (const time_span_c& d) const;
    time_span_c operator - (const time_span_c& d) const;
    time_span_c& operator += (const time_span_c& d);
    time_span_c& operator -= (const time_span_c& d);

    time_span_c operator + (time_diff_t microseconds) const;
    time_span_c operator - (time_diff_t microseconds) const;
    time_span_c& operator += (time_diff_t microseconds);
    time_span_c& operator -= (time_diff_t microseconds);

    /**
    * Returns the number of days.
    */
    int days () const;
    /**
    * Returns the number of hours (0 to 23).
    */
    int hours () const;
    /**
    * Returns the total number of hours.
    */
    int total_hours () const;
    /**
    * Returns the number of minutes (0 to 59).
    */
    int minutes () const;
    /**
    * Returns the total number of minutes.
    */
    int total_minutes () const;
    /**
    * Returns the number of seconds (0 to 59).
    */
    int seconds () const;
    /**
    * Returns the total number of seconds.
    */
    int total_seconds () const;
    /**
    * Returns the number of milliseconds (0 to 999).
    */
    int milliseconds () const;
    /**
    * Returns the total number of milliseconds.
    */
    time_diff_t total_milliseconds () const;
    /**
    * Returns the fractions of a millisecond
    * in microseconds (0 to 999).
    */
    int microseconds () const;
    /**
    * Returns the fractions of a second
    * in microseconds (0 to 999999).
    */
    int useconds () const;
    /**
    * Returns the total number of microseconds.
    */
    time_diff_t total_microseconds () const;
    

    static const time_diff_t MILLISECONDS; /**< The number of microseconds in a millisecond. */
    static const time_diff_t SECONDS;      /**< The number of microseconds in a second. */
    static const time_diff_t MINUTES;      /**< The number of microseconds in a minute. */
    static const time_diff_t HOURS;        /**< The number of microseconds in a hour. */
    static const time_diff_t DAYS;         /**< The number of microseconds in a day. */

  private:
    time_diff_t m_span;
  };


  void swap (time_span_c& s1, time_span_c& s2);

} 


#endif 
