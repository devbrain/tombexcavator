/**
 * @file   local_date_time.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Thu Dec 24 20:08:55 2009
 * 
 * @brief  local_date_time_c
 * 
 */

#ifndef __ABL_DATE_TIME_LOCAL_DATE_TIME_HPP__
#define __ABL_DATE_TIME_LOCAL_DATE_TIME_HPP__


#include "abl/abl_export.h"
#include "abl/date_time/date_time.hpp"


namespace abl 
{
	/// This class represents an instant in local time
	/// (as opposed to UTC), expressed in years, months, days, 
	/// hours, minutes, seconds and milliseconds based on the 
	/// Gregorian calendar.
	///
	/// In addition to the date and time, the class also 
	/// maintains a time zone differential, which denotes
	/// the difference in seconds from UTC to local time,
	/// i.e. UTC = local time - time zone differential.
	///
	/// Although local_date_time_c supports relational and arithmetic
	/// operators, all date/time comparisons and date/time arithmetics
	/// should be done in UTC, using the date_time_c or time_stamp_c
	/// class for better performance. The relational operators
	/// normalize the dates/times involved to UTC before carrying out
	/// the comparison.
	///
	/// The time zone differential is based on the input date and
	/// time and current time zone. A number of constructors accept
	/// an explicit time zone differential parameter. These should
	/// not be used since daylight savings time processing is impossible
	/// since the time zone is unknown. Each of the constructors
	/// accepting a tzd parameter have been marked as deprecated and
	/// may be removed in a future revision.
class ABL_API local_date_time_c
{
public:
  /// Creates a local_date_time_c with the current date/time 
		/// for the current time zone.
	local_date_time_c();
		
  		/// Creates a local_date_time_c for the given Gregorian local date and time.
		///   * year is from 0 to 9999.
		///   * month is from 1 to 12.
		///   * day is from 1 to 31.
		///   * hour is from 0 to 23.
		///   * minute is from 0 to 59.
		///   * second is from 0 to 59.
		///   * millisecond is from 0 to 999.
		///   * microsecond is from 0 to 999.

	local_date_time_c(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microsecond = 0);

	//@ deprecated
  		/// Creates a local_date_time_c for the given Gregorian date and time in the
		/// time zone denoted by the time zone differential in tzd.
		///   * tzd is in seconds.
		///   * year is from 0 to 9999.
		///   * month is from 1 to 12.
		///   * day is from 1 to 31.
		///   * hour is from 0 to 23.
		///   * minute is from 0 to 59.
		///   * second is from 0 to 59.
		///   * millisecond is from 0 to 999.
		///   * microsecond is from 0 to 999.
	local_date_time_c(int tzd, int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond);

  		/// Creates a local_date_time_c from the UTC time given in dateTime,
		/// using the time zone differential of the current time zone.
	local_date_time_c(const date_time_c& dateTime);


	//@ deprecated
  		/// Creates a local_date_time_c from the UTC time given in dateTime,
		/// using the given time zone differential. Adjusts dateTime
		/// for the given time zone differential.
	local_date_time_c(int tzd, const date_time_c& dateTime);


	//@ deprecated
  		/// Creates a local_date_time_c from the UTC time given in dateTime,
		/// using the given time zone differential. If adjust is true, 
		/// adjusts dateTime for the given time zone differential.
	local_date_time_c(int tzd, const date_time_c& dateTime, bool adjust);

  		/// Creates a local_date_time_c for the given Julian day in the local time zone.
	local_date_time_c(double julianDay);


	//@ deprecated
  		/// Creates a local_date_time_c for the given Julian day in the time zone
		/// denoted by the time zone differential in tzd.
	local_date_time_c(int tzd, double julianDay);

  		/// Copy constructor. Creates the local_date_time_c from another one.
	local_date_time_c(const local_date_time_c& dateTime);

		/// Destroys the local_date_time_c.		
	~local_date_time_c();

  		/// Assigns another local_date_time_c.
	local_date_time_c& operator = (const local_date_time_c& dateTime);

  		/// Assigns a timestamp
	local_date_time_c& operator = (const time_stamp_c& timestamp);

  		/// Assigns a Julian day in the local time zone.
	local_date_time_c& operator = (double julianDay);
  
  		/// Assigns a Gregorian local date and time.
		///   * year is from 0 to 9999.
		///   * month is from 1 to 12.
		///   * day is from 1 to 31.
		///   * hour is from 0 to 23.
		///   * minute is from 0 to 59.
		///   * second is from 0 to 59.
		///   * millisecond is from 0 to 999.
		///   * microsecond is from 0 to 999.
	local_date_time_c& assign(int year, int month, int day, int hour = 0, int minute = 0, int second = 0, int millisecond = 0, int microseconds = 0);


	//@ deprecated
  		/// Assigns a Gregorian local date and time in the time zone denoted by
		/// the time zone differential in tzd.
		///   * tzd is in seconds.
		///   * year is from 0 to 9999.
		///   * month is from 1 to 12.
		///   * day is from 1 to 31.
		///   * hour is from 0 to 23.
		///   * minute is from 0 to 59.
		///   * second is from 0 to 59.
		///   * millisecond is from 0 to 999.
		///   * microsecond is from 0 to 999.

	local_date_time_c& assign(int tzd, int year, int month, int day, int hour, int minute, int second, int millisecond, int microseconds);

	//@ deprecated
  		/// Assigns a Julian day in the time zone denoted by the
		/// time zone differential in tzd.
	local_date_time_c& assign(int tzd, double julianDay);
  		/// Swaps the local_date_time_c with another one.
	void swap(local_date_time_c& dateTime);

  		/// Returns the year.
	int year() const;

  		/// Returns the month (1 to 12).
	int month() const;

  		/// Returns the week number within the year.
		/// FirstDayOfWeek should be either SUNDAY (0) or MONDAY (1).
		/// The returned week number will be from 0 to 53. Week number 1 is the week 
		/// containing January 4. This is in accordance to ISO 8601.
		/// 
		/// The following example assumes that firstDayOfWeek is MONDAY. For 2005, which started
		/// on a Saturday, week 1 will be the week starting on Monday, January 3.
		/// January 1 and 2 will fall within week 0 (or the last week of the previous year).
		///
		/// For 2007, which starts on a Monday, week 1 will be the week startung on Monday, January 1.
		/// There will be no week 0 in 2007.
	int week(int firstDayOfWeek = date_time_c::MONDAY) const;

  		/// Returns the day witin the month (1 to 31).
	int day() const;

  		/// Returns the weekday (0 to 6, where
		/// 0 = Sunday, 1 = Monday, ..., 6 = Saturday).
	int dayOfWeek() const;
  
		/// Returns the number of the day in the year.
		/// January 1 is 1, February 1 is 32, etc.
	int dayOfYear() const;

  		/// Returns the hour (0 to 23).
	int hour() const;

  		/// Returns the hour (0 to 12).
	int hourAMPM() const;

  		/// Returns true if hour < 12;
	bool isAM() const;

  		/// Returns true if hour >= 12.
	bool isPM() const;

  		/// Returns the minute (0 to 59).
	int minute() const;

  		/// Returns the second (0 to 59).
	int second() const;

  		/// Returns the millisecond (0 to 999)
	int millisecond() const;

  		/// Returns the microsecond (0 to 999)
	int microsecond() const;

  		/// Returns the julian day for the date.
	double julianDay() const;

  		/// Returns the time zone differential.
	int tzd() const;

  		/// Returns the UTC equivalent for the local date and time.
	date_time_c utc() const;

  		/// Returns the date and time expressed as a time_stamp_c.
	time_stamp_c timestamp() const;

  		/// Returns the UTC equivalent for the local date and time.
	time_stamp_c::UtcTimeVal utcTime() const;


	bool operator == (const local_date_time_c& dateTime) const;	
	bool operator != (const local_date_time_c& dateTime) const;	
	bool operator <  (const local_date_time_c& dateTime) const;	
	bool operator <= (const local_date_time_c& dateTime) const;	
	bool operator >  (const local_date_time_c& dateTime) const;	
	bool operator >= (const local_date_time_c& dateTime) const;	

	local_date_time_c  operator +  (const time_span_c& span) const;
	local_date_time_c  operator -  (const time_span_c& span) const;
	time_span_c       operator -  (const local_date_time_c& dateTime) const;
	local_date_time_c& operator += (const time_span_c& span);
	local_date_time_c& operator -= (const time_span_c& span);

protected:
	local_date_time_c(time_stamp_c::UtcTimeVal utcTime, time_stamp_c::time_diff_t diff, int tzd);
  
		/// Recalculate the tzd based on the _dateTime member based
		/// on the current timezone using the Standard C runtime functions.
		/// If adjust is true, then adjustForTzd() is called after the
		/// differential is calculated.
	void determineTzd(bool adjust = false);

  		/// Adjust the _dateTime member based on the _tzd member.
	void adjustForTzd();

  		/// Determine the DST offset for the current date/time.
	std::time_t dstOffset(int& dstOffset) const;

	
private:
	date_time_c _dateTime;
	int      _tzd;
	
	friend class date_time_formatter_c;
	friend class date_time_parser_c;
};


//
// inlines
//
inline int local_date_time_c::year() const
{
	return _dateTime.year();
}


inline int local_date_time_c::month() const
{
	return _dateTime.month();
}


inline int local_date_time_c::week(int firstDayOfWeek) const
{
	return _dateTime.week(firstDayOfWeek);
}


inline int local_date_time_c::day() const
{
	return _dateTime.day();
}

	
inline int local_date_time_c::dayOfWeek() const
{
	return _dateTime.dayOfWeek();
}


inline int local_date_time_c::dayOfYear() const
{
	return _dateTime.dayOfYear();
}


inline int local_date_time_c::hour() const
{
	return _dateTime.hour();
}

	
inline int local_date_time_c::hourAMPM() const
{
	return _dateTime.hourAMPM();
}


inline bool local_date_time_c::isAM() const
{
	return _dateTime.isAM();
}


inline bool local_date_time_c::isPM() const
{
	return _dateTime.isPM();
}

	
inline int local_date_time_c::minute() const
{
	return _dateTime.minute();
}

	
inline int local_date_time_c::second() const
{
	return _dateTime.second();
}

	
inline int local_date_time_c::millisecond() const
{
	return _dateTime.millisecond();
}


inline int local_date_time_c::microsecond() const
{
	return _dateTime.microsecond();
}


inline double local_date_time_c::julianDay() const
{
	return _dateTime.julianDay();
}


inline int local_date_time_c::tzd() const
{
	return _tzd;
}


inline time_stamp_c local_date_time_c::timestamp() const
{
	return time_stamp_c::fromUtcTime(_dateTime.utcTime());
}


inline time_stamp_c::UtcTimeVal local_date_time_c::utcTime() const
{
	return _dateTime.utcTime() - ((time_stamp_c::time_diff_t) _tzd)*10000000;
}


inline void local_date_time_c::adjustForTzd()
{
	_dateTime += time_span_c(((time_stamp_c::time_diff_t) _tzd)*time_span_c::SECONDS);
}


inline void swap(local_date_time_c& d1, local_date_time_c& d2)
{
	d1.swap(d2);
}


} // namespace Poco


#endif // Foundation_local_date_time_c_INCLUDED
