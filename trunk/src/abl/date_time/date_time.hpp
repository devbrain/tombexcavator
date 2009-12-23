/**
 * @file   date_time.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 16:37:58 2009
 * 
 * @brief  Definition of the date_time_c class.
 */

#ifndef __ABL_DATE_TIME_DATE_TIME_HPP__
#define __ABL_DATE_TIME_DATE_TIME_HPP__


#include "abl/abl_export.h"
#include "abl/date_time/time_stamp.hpp"
#include "abl/date_time/time_span.hpp"


namespace abl 
{
  /**
   * This class represents an instant in time, expressed
   * in years, months, days, hours, minutes, seconds
   * and milliseconds based on the Gregorian calendar.
   * The class is mainly useful for conversions between
   * UTC, Julian day and Gregorian calendar dates.
   *
   * The date and time stored in a DateTime is always in UTC
   * (Coordinated Universal Time) and thus independent of the 
   * timezone in effect on the system.
   *
   * Conversion calculations are based on algorithms
   * collected and described by Peter Baum at
   * http://vsg.cape.com/~pbaum/date/date0.htm
   *
   * Internally, this class stores a date/time in two
   * forms (UTC and broken down) for performance reasons. Only use 
   * this class for conversions between date/time representations.
   * Use the time_stamp_c class for everything else.
   *
   * Notes:
   *   * Zero is a valid year (in accordance with ISO 8601 and astronomical year numbering)
   *   * Year zero (0) is a leap year
   *   * Negative years (years preceding 1 BC) are not supported
   *
   * For more information, please see:
   *   * http://en.wikipedia.org/wiki/Gregorian_Calendar
   *   * http://en.wikipedia.org/wiki/Julian_day
   *   * http://en.wikipedia.org/wiki/UTC
   *   * http://en.wikipedia.org/wiki/ISO_8601
   */
  class ABL_API date_time_c
  {
  public:
    /**
     * Symbolic names for month numbers  (1 to 12).
     */
    enum months_t
      {
	JANUARY = 1,
	FEBRUARY,
	MARCH,
	APRIL,
	MAY,
	JUNE,
	JULY,
	AUGUST,
	SEPTEMBER,
	OCTOBER,
	NOVEMBER,
	DECEMBER
      };
    /**
     * Symbolic names for week day numbers  (0 to 6).
     */
    enum days_of_week_t
      {
	SUNDAY = 0,
	MONDAY,
	TUESDAY,
	WEDNESDAY,
	THURSDAY,
	FRIDAY,
	SATURDAY
      };
    /**
     * Creates a date_time_c for the current date and time.		
     */
    date_time_c ();
    /**
     * Creates a date_time_c for the date and time given in
     * a time_stamp_c.
     */
    date_time_c (const time_stamp_c& timestamp);
    /**
     * Creates a date_time_c for the given Gregorian date and time.
     *   * year is from 0 to 9999.
     *   * month is from 1 to 12.
     *   * day is from 1 to 31.
     *   * hour is from 0 to 23.
     *   * minute is from 0 to 59.
     *   * second is from 0 to 59.
     *   * millisecond is from 0 to 999.
     *   * microsecond is from 0 to 999.
     */	
    date_time_c (int year, int month, int day, int hour = 0, 
		 int minute = 0, int second = 0, int millisecond = 0, 
		 int microsecond = 0);
    /**
     * Creates a date_time_c for the given Julian day.
     */
    date_time_c (double julianDay);
    /**
     * Creates a date_time_c from an utc_time_val_t and a time_diff_t.
     *
     * Mainly used internally by date_time_c and friends.
     */
    date_time_c (time_stamp_c::utc_time_val_t utcTime, 
		 time_stamp_c::time_diff_t diff);
    /**
     * Copy constructor. Creates the date_time_c from another one.
     */
    date_time_c (const date_time_c& dateTime);
    /**
     * Destroys the date_time_c.
     */
    ~date_time_c ();
    /**	
     * Assigns another date_time_c.
     */
    date_time_c& operator =  (const date_time_c& dateTime);
    /**	
     * Assigns a time_stamp_c.
     */
    date_time_c& operator =  (const time_stamp_c& timestamp);
    /*
     * Assigns a Julian day.
     */
    date_time_c& operator =  (double julianDay);
    /**
     * Assigns a Gregorian date and time.
     *   * year is from 0 to 9999.
     *   * month is from 1 to 12.
     *   * day is from 1 to 31.
     *   * hour is from 0 to 23.
     *   * minute is from 0 to 59.
     *   * second is from 0 to 59.
     *   * millisecond is from 0 to 999.
     *   * microsecond is from 0 to 999.
     */
    date_time_c& assign (int year, int month, int day, 
			 int hour = 0, int minute = 0, 
			 int second = 0, int millisecond = 0, 
			 int microseconds = 0);
    /**
     * Swaps the date_time_c with another one.
     */
    void swap (date_time_c& dateTime);
    /**
     * Returns the year.
     */
    int year () const;
    /**
     * Returns the month  (1 to 12).
     */
    int month () const;
    /**
     * Returns the week number within the year.
     * FirstDayOfWeek should be either SUNDAY  (0) or MONDAY  (1).
     * The returned week number will be from 0 to 53. Week number 1 is the week 
     * containing January 4. This is in accordance to ISO 8601.
     * 
     * The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
     * on a Saturday, week 1 will be the week starting on Monday, January 3.
     * January 1 and 2 will fall within week 0  (or the last week of the previous year).
     *
     * For 2007, which starts on a Monday, week 1 will be the week startung on Monday, January 1.
     * There will be no week 0 in 2007.	
     */
    int week (int firstDayOfWeek = MONDAY) const;
    /**
     * Returns the day witin the month  (1 to 31).
     */
    int day () const;
    /**
     * Returns the weekday  (0 to 6, where
     * 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
     */
    int day_of_week () const;
    /**
     * Returns the number of the day in the year.
     * January 1 is 1, February 1 is 32, etc.
     */
    int day_of_year () const;
    /**
     * Returns the hour  (0 to 23).
     */
    int hour () const;
    /**
     * Returns the hour  (0 to 12).
     */
    int hour_am_pm () const;
    /**
     * Returns true if hour < 12;
     */
    bool is_am () const;
    /**
     * Returns true if hour >= 12.
     */
    bool is_pm () const;
    /**
     * Returns the minute  (0 to 59).
     */
    int minute () const;
    /**
     * Returns the second  (0 to 59).
     */
    int second () const;
    /**
     * Returns the millisecond  (0 to 999)
     */
    int millisecond () const;
    /**
     * Returns the microsecond  (0 to 999)
     */
    int microsecond () const;
    /**
     * Returns the julian day for the date and time.
     */
    double julian_day () const;
    /**
     * Returns the date and time expressed as a time_stamp_c.
     */
    time_stamp_c timestamp () const;
    /**
     * Returns the date and time expressed in UTC-based
     * time. UTC base time is midnight, October 15, 1582.
     * Resolution is 100 nanoseconds.
     */
    time_stamp_c::utc_time_val_t utc_time () const;

		
    bool operator ==  (const date_time_c& dateTime) const;	
    bool operator !=  (const date_time_c& dateTime) const;	
    bool operator <   (const date_time_c& dateTime) const;	
    bool operator <=  (const date_time_c& dateTime) const;	
    bool operator >   (const date_time_c& dateTime) const;	
    bool operator >=  (const date_time_c& dateTime) const;	

    date_time_c  operator +   (const time_span_c& span) const;
    date_time_c  operator -   (const time_span_c& span) const;
    time_span_c  operator -   (const date_time_c& dateTime) const;
    date_time_c& operator +=  (const time_span_c& span);
    date_time_c& operator -=  (const time_span_c& span);
    /**
     * Converts a local time into UTC, by applying the given time zone differential.
     */
    void make_utc (int tzd);
    /**
     * Converts a UTC time into a local time, by applying the given time zone differential.
     */
    void make_local (int tzd);
    /**
     * Returns true if the given year is a leap year;
     * false otherwise.
     */	
    static bool is_leap_year (int year);
    /**
     * Returns the number of days in the given month
     * and year. Month is from 1 to 12.
     */
    static int days_of_month (int year, int month);
    /**	
     * Checks if the given date and time is valid
     *  (all arguments are within a proper range).
     *
     * Returns true if all arguments are valid, false otherwise.
     */
    static bool is_valid (int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

		
  protected:	
    /**
     * Computes the Julian day for an UTC time.
     */
    static double _to_julian_day (time_stamp_c::utc_time_val_t utcTime);
    /**
     * Computes the Julian day for a gregorian calendar date and time.
     * See <http://vsg.cape.com/~pbaum/date/jdimp.htm>, section 2.3.1 for the algorithm.
     */
    static double _to_julian_day (int year, int month, int day, 
				  int hour = 0, int minute = 0, 
				  int second = 0, int millisecond = 0, 
				  int microsecond = 0);
    /**
     * Computes the UTC time for a Julian day.
     */
    static time_stamp_c::utc_time_val_t _to_utc_time (double julianDay);
    /**
     * Computes the Gregorian date for the given Julian day.
     * See <http://vsg.cape.com/~pbaum/date/injdimp.htm>, section 3.3.1 for the algorithm.
     */
    void _compute_gregorian (double julianDay);
    /**
     * Extracts the daytime  (hours, minutes, seconds, etc.) from the stored utcTime.
     */
    void _compute_daytime ();


  private:
    void _check_limit (short& lower, short& higher, short limit);
    /**
     * utility functions used to correct the overflow in computeGregorian
     */
    void _normalize ();

  private:
    time_stamp_c::utc_time_val_t m_utc_time;
    short  m_year;
    short  m_month;
    short  m_day;
    short  m_hour;
    short  m_minute;
    short  m_second;
    short  m_millisecond;
    short  m_microsecond;
  };

  void swap(date_time_c& d1, date_time_c& d2);

} 


#endif 

