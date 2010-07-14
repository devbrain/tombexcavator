#include "abl/date_time/time_span.hpp"
#include <algorithm>


namespace abl 
{


  const time_span_c::time_diff_t time_span_c::MILLISECONDS = 1000;
  const time_span_c::time_diff_t time_span_c::SECONDS      = 1000*time_span_c::MILLISECONDS;
  const time_span_c::time_diff_t time_span_c::MINUTES      =   60*time_span_c::SECONDS;
  const time_span_c::time_diff_t time_span_c::HOURS        =   60*time_span_c::MINUTES;
  const time_span_c::time_diff_t time_span_c::DAYS         =   24*time_span_c::HOURS;

  // --------------------------------------------------------------------------------------------------
  time_span_c::time_span_c ()
    : m_span (0)
  {
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::time_span_c (time_diff_t microseconds)
    : m_span (microseconds)
  {
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::time_span_c (long seconds, long microseconds)
    : m_span (time_diff_t (seconds)*SECONDS + microseconds)
  {
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::time_span_c (int days, int hours, int minutes, int seconds, int microseconds)
    : m_span (time_diff_t (microseconds) + 
	      time_diff_t (seconds)*SECONDS + 
	      time_diff_t (minutes)*MINUTES + 
	      time_diff_t (hours)*HOURS + 
	      time_diff_t (days)*DAYS)
  {
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::time_span_c (const time_span_c& timespan):
    m_span (timespan.m_span)
  {
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::~time_span_c ()
  {
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::operator =  (const time_span_c& timespan)
  {
    m_span = timespan.m_span;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::operator =  (time_diff_t microseconds)
  {
    m_span = microseconds;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::assign (int days, int hours, int minutes, int seconds, int microseconds)
  {
    m_span = 
      time_diff_t (microseconds) + 
      time_diff_t (seconds)*SECONDS + 
      time_diff_t (minutes)*MINUTES + 
      time_diff_t (hours)*HOURS + 
      time_diff_t (days)*DAYS;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::assign (long seconds, long microseconds)
  {
    m_span = time_diff_t (seconds)*SECONDS + time_diff_t (microseconds);
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  void time_span_c::swap (time_span_c& timespan)
  {
    std::swap (m_span, timespan.m_span);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c time_span_c::operator +  (const time_span_c& d) const
  {
    return time_span_c (m_span + d.m_span);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c time_span_c::operator -  (const time_span_c& d) const
  {
    return time_span_c (m_span - d.m_span);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::operator +=  (const time_span_c& d)
  {
    m_span += d.m_span;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::operator -=  (const time_span_c& d)
  {
    m_span -= d.m_span;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c time_span_c::operator +  (time_diff_t microseconds) const
  {
    return time_span_c (m_span + microseconds);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c time_span_c::operator -  (time_diff_t microseconds) const
  {
    return time_span_c (m_span - microseconds);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::operator +=  (time_diff_t microseconds)
  {
    m_span += microseconds;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c& time_span_c::operator -=  (time_diff_t microseconds)
  {
    m_span -= microseconds;
    return *this;
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::days () const
  {
    return int (m_span/DAYS);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::hours () const
  {
    return int ( (m_span/HOURS) % 24);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::total_hours () const
  {
    return int (m_span/HOURS);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::minutes () const
  {
    return int ( (m_span/MINUTES) % 60);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::total_minutes () const
  {
    return int (m_span/MINUTES);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::seconds () const
  {
    return int ( (m_span/SECONDS) % 60);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::total_seconds () const
  {
    return int (m_span/SECONDS);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::milliseconds () const
  {
    return int ( (m_span/MILLISECONDS) % 1000);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::time_diff_t time_span_c::total_milliseconds () const
  {
    return m_span/MILLISECONDS;
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::microseconds () const
  {
    return int (m_span % 1000);
  }
  // --------------------------------------------------------------------------------------------------
  int time_span_c::useconds () const
  {
    return int (m_span % 1000000);
  }
  // --------------------------------------------------------------------------------------------------
  time_span_c::time_diff_t time_span_c::total_microseconds  () const
  {
    return m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator ==  (const time_span_c& ts) const
  {
    return m_span == ts.m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator !=  (const time_span_c& ts) const
  {
    return m_span != ts.m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator >   (const time_span_c& ts) const
  {
    return m_span > ts.m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator >=  (const time_span_c& ts) const
  {
    return m_span >= ts.m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator <   (const time_span_c& ts) const
  {
    return m_span < ts.m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator <=  (const time_span_c& ts) const
  {
    return m_span <= ts.m_span;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator ==  (time_diff_t microseconds) const
  {
    return m_span == microseconds;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator !=  (time_diff_t microseconds) const
  {
    return m_span != microseconds;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator >   (time_diff_t microseconds) const
  {
    return m_span > microseconds;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator >=  (time_diff_t microseconds) const
  {
    return m_span >= microseconds;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator <   (time_diff_t microseconds) const
  {
    return m_span < microseconds;
  }
  // --------------------------------------------------------------------------------------------------
  bool time_span_c::operator <=  (time_diff_t microseconds) const
  {
    return m_span <= microseconds;
  }
  // --------------------------------------------------------------------------------------------------
  void swap (time_span_c& s1, time_span_c& s2)
  {
    s1.swap (s2);
  }

} 

