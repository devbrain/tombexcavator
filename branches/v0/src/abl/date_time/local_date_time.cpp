#include "abl/date_time/local_date_time.hpp"
#include "abl/date_time/time_zone.hpp"
#include "abl/date_time/time_span.hpp"
#include <algorithm>
#include <ctime>


namespace abl 
{
  local_date_time_c::local_date_time_c ()
  {
    _determine_tzd (true);
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (int year, int month, int day, 
					int hour, int minute, int second, 
					int millisecond, int microsecond)
    : m_date_time (year, month, day, hour, minute, second, millisecond, microsecond)
  {
    _determine_tzd ();
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (int tzd, int year, int month, int day, 
					int hour, int minute, int second, 
					int millisecond, int microsecond)
    : m_date_time (year, month, day, hour, minute, second, millisecond, microsecond),
      m_tzd (tzd)
  {
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (double julianDay):
    m_date_time (julianDay)
  {
    _determine_tzd (true);
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (int tzd, double julianDay)
    : m_date_time (julianDay),
      m_tzd (tzd)
  {
    _adjust_for_tzd ();
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (const date_time_c& dateTime)
    : m_date_time (dateTime)
  {
    _determine_tzd (true);
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (int tzd, const date_time_c& dateTime)
    : m_date_time (dateTime),
      m_tzd (tzd)
  {
    _adjust_for_tzd ();
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (int tzd, const date_time_c& dateTime, bool adjust)
    : m_date_time (dateTime),
      m_tzd (tzd)
  {
    if  (adjust)
      _adjust_for_tzd ();
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (const local_date_time_c& dateTime)
    : m_date_time (dateTime.m_date_time),
      m_tzd (dateTime.m_tzd)
  {
  }
  // ----------------------------------------------------------
  local_date_time_c::local_date_time_c (time_stamp_c::utc_time_val_t utc_time, 
					time_stamp_c::time_diff_t diff, int tzd)
    : m_date_time (utc_time, diff),
      m_tzd (tzd)
  {
    _adjust_for_tzd ();
  }
  // ----------------------------------------------------------
  local_date_time_c::~local_date_time_c ()
  {
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::operator =  (const local_date_time_c& dateTime)
  {
    if  (&dateTime != this)
      {
	m_date_time = dateTime.m_date_time;
	m_tzd      = dateTime.m_tzd;
      }
    return *this;
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::operator =  (const time_stamp_c& timestamp)
  {
    if  (timestamp != this->timestamp ())
      {
	m_date_time = timestamp;
	_determine_tzd (true);
      }
    return *this;
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::operator =  (double julianDay)
  {
    m_date_time = julianDay;
    _determine_tzd (true);
    return *this;
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::assign (int year, int month, int day, int hour, 
						int minute, int second, int millisecond, 
						int microseconds)
  {
    m_date_time.assign (year, month, day, hour, minute, second, millisecond, microseconds);
    _determine_tzd (false);
    return *this;
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::assign (int tzd, int year, int month, int day, 
						int hour, int minute, int second, 
						int millisecond, int microseconds)
  {
    m_date_time.assign (year, month, day, hour, minute, second, millisecond, microseconds);
    m_tzd = tzd;
    return *this;
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::assign (int tzd, double julianDay)
  {
    m_tzd      = tzd;
    m_date_time = julianDay;
    _adjust_for_tzd ();
    return *this;
  }
  // ----------------------------------------------------------
  void local_date_time_c::swap (local_date_time_c& dateTime)
  {
    m_date_time.swap (dateTime.m_date_time);
    std::swap (m_tzd, dateTime.m_tzd);
  }
  // ----------------------------------------------------------
  date_time_c local_date_time_c::utc () const
  {
    return date_time_c (m_date_time.utc_time (), - ( (time_stamp_c::time_diff_t) m_tzd)*time_span_c::SECONDS);
  }
  // ----------------------------------------------------------
  bool local_date_time_c::operator ==  (const local_date_time_c& dateTime) const
  {
    return utc_time () == dateTime.utc_time ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::operator !=  (const local_date_time_c& dateTime) const	
  {
    return utc_time () != dateTime.utc_time ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::operator <   (const local_date_time_c& dateTime) const	
  {
    return utc_time () < dateTime.utc_time ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::operator <=  (const local_date_time_c& dateTime) const	
  {
    return utc_time () <= dateTime.utc_time ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::operator >   (const local_date_time_c& dateTime) const	
  {
    return utc_time () > dateTime.utc_time ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::operator >=  (const local_date_time_c& dateTime) const	
  {
    return utc_time () >= dateTime.utc_time ();
  }
  // ----------------------------------------------------------
  local_date_time_c local_date_time_c::operator +  (const time_span_c& span) const
  {
    // First calculate the adjusted UTC time, then calculate the
    // locally adjusted time by constructing a new local_date_time_c.
    date_time_c tmp (utc_time (), span.total_microseconds ());
    return local_date_time_c (tmp);
  }
  // ----------------------------------------------------------
  local_date_time_c local_date_time_c::operator -  (const time_span_c& span) const
  {
    // First calculate the adjusted UTC time, then calculate the
    // locally adjusted time by constructing a new local_date_time_c.
    date_time_c tmp (utc_time (), -span.total_microseconds ());
    return local_date_time_c (tmp);
  }
  // ----------------------------------------------------------
  time_span_c local_date_time_c::operator -  (const local_date_time_c& dateTime) const
  {
    return time_span_c ( (utc_time () - dateTime.utc_time ())/10);
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::operator +=  (const time_span_c& span)
  {
    // Use the same trick as in operator+. Create a UTC time, adjust
    // it for the span, and convert back to local_date_time_c. This will
    // recalculate the tzd correctly in the case where the addition
    // crosses a DST boundary.
    *this = date_time_c (utc_time (), span.total_microseconds ());
    return *this;
  }
  // ----------------------------------------------------------
  local_date_time_c& local_date_time_c::operator -=  (const time_span_c& span)
  {
    // Use the same trick as in operator-. Create a UTC time, adjust
    // it for the span, and convert back to local_date_time_c. This will
    // recalculate the tzd correctly in the case where the subtraction
    // crosses a DST boundary.
    *this = date_time_c (utc_time (), -span.total_microseconds ());
    return *this;
  }
  // ----------------------------------------------------------
  void local_date_time_c::_determine_tzd (bool adjust)
  {
    if  (adjust)
      {
	std::time_t epochTime = m_date_time.timestamp ().epoch_time ();
#if defined (_WIN32) || defined (ABL_NO_POSIX_TSF)
	std::tm* broken = std::localtime (&epochTime);
	m_tzd =  (time_zone_c::utc_offset () +  ( (broken->tm_isdst == 1) ? 3600 : 0));
#else
	std::tm broken;
	localtime_r (&epochTime, &broken);
	m_tzd =  (time_zone_c::utc_offset () +  ( (broken.tm_isdst == 1) ? 3600 : 0));
#endif
	_adjust_for_tzd ();
      }
    else
      {
	int dst;
	_dst_offset (dst);
	m_tzd =  (time_zone_c::utc_offset () + dst);
      }
  }
  // ----------------------------------------------------------
  std::time_t local_date_time_c::_dst_offset (int& _dst_offset) const
  {
    std::time_t local;
    std::tm     broken;

    broken.tm_year  =  (m_date_time.year () - 1900);
    broken.tm_mon   =  (m_date_time.month () - 1);
    broken.tm_mday  = m_date_time.day ();
    broken.tm_hour  = m_date_time.hour ();
    broken.tm_min   = m_date_time.minute ();
    broken.tm_sec   = m_date_time.second ();
    broken.tm_isdst = -1;
    local = std::mktime (&broken);
	
    _dst_offset =  (broken.tm_isdst == 1) ? 3600 : 0;
    return local;
  }
  // ----------------------------------------------------------
  int local_date_time_c::year () const
  {
    return m_date_time.year ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::month () const
  {
    return m_date_time.month ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::week (int firstDayOfWeek) const
  {
    return m_date_time.week (firstDayOfWeek);
  }
  // ----------------------------------------------------------
  int local_date_time_c::day () const
  {
    return m_date_time.day ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::day_of_week () const
  {
    return m_date_time.day_of_week ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::day_of_year () const
  {
    return m_date_time.day_of_year ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::hour () const
  {
    return m_date_time.hour ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::hour_am_pm () const
  {
    return m_date_time.hour_am_pm ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::is_am () const
  {
    return m_date_time.is_am ();
  }
  // ----------------------------------------------------------
  bool local_date_time_c::is_pm () const
  {
    return m_date_time.is_pm ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::minute () const
  {
    return m_date_time.minute ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::second () const
  {
    return m_date_time.second ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::millisecond () const
  {
    return m_date_time.millisecond ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::microsecond () const
  {
    return m_date_time.microsecond ();
  }
  // ----------------------------------------------------------
  double local_date_time_c::julianDay () const
  {
    return m_date_time.julian_day ();
  }
  // ----------------------------------------------------------
  int local_date_time_c::tzd () const
  {
    return m_tzd;
  }
  // ----------------------------------------------------------
  time_stamp_c local_date_time_c::timestamp () const
  {
    return time_stamp_c::from_utc_time (m_date_time.utc_time ());
  }
  // ----------------------------------------------------------
  time_stamp_c::utc_time_val_t local_date_time_c::utc_time () const
  {
    return m_date_time.utc_time () -  ( (time_stamp_c::time_diff_t) m_tzd)*10000000;
  }
  // ----------------------------------------------------------
  void local_date_time_c::_adjust_for_tzd ()
  {
    m_date_time += time_span_c ( ( (time_stamp_c::time_diff_t) m_tzd)*time_span_c::SECONDS);
  }
  // ----------------------------------------------------------
  void swap (local_date_time_c& d1, local_date_time_c& d2)
  {
    d1.swap (d2);
  }


} 

