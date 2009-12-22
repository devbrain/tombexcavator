#include "abl/date_time/time_stamp.hpp"
#include "abl/exception.hpp"

#include <algorithm>
#if !defined(_WIN32)
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/times.h>
#else
#include <windows.h>
#endif


namespace abl 
{
  time_stamp_c::time_stamp_c()
  {
    update();
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::time_stamp_c(time_val_t tv)
  {
    m_ts = tv;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::time_stamp_c(const time_stamp_c& other)
  {
    m_ts = other.m_ts;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::~time_stamp_c()
  {
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c& time_stamp_c::operator = (const time_stamp_c& other)
  {
    m_ts = other.m_ts;
    return *this;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c& time_stamp_c::operator = (time_val_t tv)
  {
    m_ts = tv;
    return *this;
  }
  // ---------------------------------------------------------------------------------
  void time_stamp_c::swap(time_stamp_c& timestamp)
  {
    std::swap(m_ts, timestamp.m_ts);
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c time_stamp_c::from_epoch_time (std::time_t t)
  {
    return time_stamp_c(time_val_t(t)*resolution());
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c time_stamp_c::from_utc_time (utc_time_val_t val)
  {
    val -= (time_diff_t(0x01b21dd2) << 32) + 0x13814000;
    val /= 10;
    return time_stamp_c(val);
  }
  // ---------------------------------------------------------------------------------
  void time_stamp_c::update ()
  {
#if defined(_WIN32)

    FILETIME ft;
    GetSystemTimeAsFileTime(&ft);
    ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
    epoch.LowPart  = 0xD53E8000;
    epoch.HighPart = 0x019DB1DE;

    ULARGE_INTEGER ts;
    ts.LowPart  = ft.dwLowDateTime;
    ts.HighPart = ft.dwHighDateTime;
    ts.QuadPart -= epoch.QuadPart;
    m_ts = ts.QuadPart/10;
#else
    struct timeval tv;
    if (gettimeofday(&tv, NULL))
      throw system_exception_c ("cannot get time of day");
    m_ts = time_val_t(tv.tv_sec)*resolution() + tv.tv_usec;
	
#endif
  }


#if defined(_WIN32)
  time_stamp_c time_stamp_c::from_file_time_np (types::uint32_t fileTimeLow, types::uint32_t fileTimeHigh)
  {
    ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
    epoch.LowPart  = 0xD53E8000;
    epoch.HighPart = 0x019DB1DE;
	
    ULARGE_INTEGER ts;
    ts.LowPart  = fileTimeLow;
    ts.HighPart = fileTimeHigh;
    ts.QuadPart -= epoch.QuadPart;

    return time_stamp_c(ts.QuadPart/10);
  }
  // ---------------------------------------------------------------------------------
  void time_stamp_c::to_file_time_np (types::uint32_t& fileTimeLow, types::uint32_t& fileTimeHigh) const
  {
    ULARGE_INTEGER epoch; // UNIX epoch (1970-01-01 00:00:00) expressed in Windows NT FILETIME
    epoch.LowPart  = 0xD53E8000;
    epoch.HighPart = 0x019DB1DE;

    ULARGE_INTEGER ts;
    ts.QuadPart  = m_ts*10;
    ts.QuadPart += epoch.QuadPart;
    fileTimeLow  = ts.LowPart;
    fileTimeHigh = ts.HighPart;
  }
#endif
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::operator == (const time_stamp_c& ts) const
  {
    return m_ts == ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::operator != (const time_stamp_c& ts) const
  {
    return m_ts != ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::operator >  (const time_stamp_c& ts) const
  {
    return m_ts > ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::operator >= (const time_stamp_c& ts) const
  {
    return m_ts >= ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::operator <  (const time_stamp_c& ts) const
  {
    return m_ts < ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::operator <= (const time_stamp_c& ts) const
  {
    return m_ts <= ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c time_stamp_c::operator + (time_stamp_c::time_diff_t d) const
  {
    return time_stamp_c(m_ts + d);
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c time_stamp_c::operator - (time_stamp_c::time_diff_t d) const
  {
    return time_stamp_c (m_ts - d);
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::time_diff_t time_stamp_c::operator - (const time_stamp_c& ts) const
  {
    return m_ts - ts.m_ts;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c& time_stamp_c::operator += (time_stamp_c::time_diff_t d)
  {
    m_ts += d;
    return *this;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c& time_stamp_c::operator -= (time_stamp_c::time_diff_t d)
  {
    m_ts -= d;
    return *this;
  }
  // ---------------------------------------------------------------------------------
  std::time_t time_stamp_c::epoch_time () const
  {
    return std::time_t(m_ts/resolution());
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::utc_time_val_t time_stamp_c::utc_time () const
  {
    return m_ts*10 + (time_diff_t(0x01b21dd2) << 32) + 0x13814000;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::time_val_t time_stamp_c::epoch_microseconds () const
  {
    return m_ts;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::time_diff_t time_stamp_c::elapsed() const
  {
    time_stamp_c now;
    return now - *this;
  }
  // ---------------------------------------------------------------------------------
  bool time_stamp_c::is_elapsed(time_stamp_c::time_diff_t interval) const
  {
    time_stamp_c now;
    time_stamp_c::time_diff_t diff = now - *this;
    return diff >= interval;
  }
  // ---------------------------------------------------------------------------------
  time_stamp_c::time_val_t time_stamp_c::resolution()
  {
    return 1000000;
  }
  // ---------------------------------------------------------------------------------
  void swap(time_stamp_c& s1, time_stamp_c& s2)
  {
    s1.swap(s2);
  }

} 
