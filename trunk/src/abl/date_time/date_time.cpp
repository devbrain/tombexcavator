#include "abl/date_time/date_time.hpp"
#include <algorithm>
#include <cmath>


namespace abl
{


  double date_time_c::toJulianDay(time_stamp_c::utc_time_val_t utcTime)
  {
    double utcDays = double(utcTime)/864000000000.0;
    return utcDays + 2299160.5; // first day of Gregorian reform (Oct 15 1582)
  }


  time_stamp_c::utc_time_val_t date_time_c::toUtcTime(double julianDay)
  {
    return time_stamp_c::utc_time_val_t((julianDay - 2299160.5)*864000000000.0);
  }


  date_time_c::date_time_c()
  {
    time_stamp_c now;
    m_utcTime = now.utcTime();
    computeGregorian(julianDay());
    computeDaytime();
  }


  date_time_c::date_time_c(const time_stamp_c& timestamp):
    m_utcTime(timestamp.utcTime())
  {
    computeGregorian(julianDay());
    computeDaytime();
  }

	
  date_time_c::date_time_c(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond):
    m_year(year),
    m_month(month),
    m_day(day),
    m_hour(hour),
    m_minute(minute),
    m_second(second),
    m_millisecond(millisecond),
    m_microsecond(microsecond)
  {
    poco_assert (year >= 0 && year <= 9999);
    poco_assert (month >= 1 && month <= 12);
    poco_assert (day >= 1 && day <= daysOfMonth(year, month));
    poco_assert (hour >= 0 && hour <= 23);
    poco_assert (minute >= 0 && minute <= 59);
    poco_assert (second >= 0 && second <= 59);
    poco_assert (millisecond >= 0 && millisecond <= 999);
    poco_assert (microsecond >= 0 && microsecond <= 999);
	
    m_utcTime = toUtcTime(toJulianDay(year, month, day)) + 10*(hour*time_span_c::HOURS + minute*time_span_c::MINUTES + second*time_span_c::SECONDS + millisecond*time_span_c::MILLISECONDS + microsecond);
  }


  date_time_c::date_time_c(double julianDay):
    m_utcTime(toUtcTime(julianDay))
  {
    computeGregorian(julianDay);
  }


  date_time_c::date_time_c(time_stamp_c::utc_time_val_t utcTime, time_stamp_c::TimeDiff diff):
    m_utcTime(utcTime + diff*10)
  {
    computeGregorian(julianDay());
    computeDaytime();
  }


  date_time_c::date_time_c(const date_time_c& dateTime):
    m_utcTime(dateTime.m_utcTime),
    m_year(dateTime.m_year),
    m_month(dateTime.m_month),
    m_day(dateTime.m_day),
    m_hour(dateTime.m_hour),
    m_minute(dateTime.m_minute),
    m_second(dateTime.m_second),
    m_millisecond(dateTime.m_millisecond),
    m_microsecond(dateTime.m_microsecond)
  {
  }


  date_time_c::~date_time_c()
  {
  }


  date_time_c& date_time_c::operator = (const date_time_c& dateTime)
  {
    if (&dateTime != this)
      {
	m_utcTime     = dateTime.m_utcTime;
	m_year        = dateTime.m_year;
	m_month       = dateTime.m_month;
	m_day         = dateTime.m_day;
	m_hour        = dateTime.m_hour;
	m_minute      = dateTime.m_minute;
	m_second      = dateTime.m_second;
	m_millisecond = dateTime.m_millisecond;
	m_microsecond = dateTime.mm_microsecond;
      }
    return *this;
  }

	
  date_time_c& date_time_c::operator = (const time_stamp_c& timestamp)
  {
    m_utcTime = timestamp.utcTime();
    computeGregorian(julianDay());
    computeDaytime();
    return *this;
  }


  date_time_c& date_time_c::operator = (double julianDay)
  {
    m_utcTime = toUtcTime(julianDay);
    computeGregorian(julianDay);
    return *this;
  }


  date_time_c& date_time_c::assign(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
  {
    poco_assert (year >= 0 && year <= 9999);
    poco_assert (month >= 1 && month <= 12);
    poco_assert (day >= 1 && day <= daysOfMonth(year, month));
    poco_assert (hour >= 0 && hour <= 23);
    poco_assert (minute >= 0 && minute <= 59);
    poco_assert (second >= 0 && second <= 59);
    poco_assert (millisecond >= 0 && millisecond <= 999);
    poco_assert (microsecond >= 0 && microsecond <= 999);

    m_utcTime     = toUtcTime(toJulianDay(year, month, day)) + 10*(hour*time_span_c::HOURS + minute*time_span_c::MINUTES + second*time_span_c::SECONDS + millisecond*time_span_c::MILLISECONDS + microsecond);
    m_year        = year;
    m_month       = month;
    m_day         = day;
    m_hour        = hour;
    m_minute      = minute;
    m_second      = second;
    m_millisecond = millisecond;
    m_microsecond = microsecond;
	
    return *this;
  }


  void date_time_c::swap(date_time_c& dateTime)
  {
    std::swap(m_utcTime, dateTime.m_utcTime);
    std::swap(m_year, dateTime.m_year);
    std::swap(m_month, dateTime.m_month);
    std::swap(m_day, dateTime.m_day);
    std::swap(m_hour, dateTime.m_hour);
    std::swap(m_minute, dateTime.m_minute);
    std::swap(m_second, dateTime.m_second);
    std::swap(m_millisecond, dateTime.m_millisecond);
    std::swap(m_microsecond, dateTime.m_microsecond);
  }


  int date_time_c::dayOfWeek() const
  {
    return int((std::floor(julianDay() + 1.5))) % 7;
  }


  int date_time_c::dayOfYear() const
  {
    int doy = 0;
    for (int month = 1; month < m_month; ++month)
      doy += daysOfMonth(m_year, month);
    doy += m_day;
    return doy;
  }


  int date_time_c::daysOfMonth(int year, int month)
  {
    poco_assert (month >= 1 && month <= 12);

    static int daysOfMonthTable[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
	
    if (month == 2 && isLeapYear(year))
      return 29;
    else
      return daysOfMonthTable[month];
  }


  bool date_time_c::isValid(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
  {
    return
      (year >= 0 && year <= 9999) &&
      (month >= 1 && month <= 12) &&
      (day >= 1 && day <= daysOfMonth(year, month)) &&
      (hour >= 0 && hour <= 23) &&
      (minute >= 0 && minute <= 59) &&
      (second >= 0 && second <= 59) &&
      (millisecond >= 0 && millisecond <= 999) &&
      (microsecond >= 0 && microsecond <= 999);
  }


  int date_time_c::week(int firstDayOfWeek) const
  {
    poco_assert (firstDayOfWeek >= 0 && firstDayOfWeek <= 6);

    /// find the first firstDayOfWeek.
      int baseDay = 1;
      while (date_time_c(m_year, 1, baseDay).dayOfWeek() != firstDayOfWeek) ++baseDay;

      int doy  = dayOfYear();
      int offs = baseDay <= 4 ? 0 : 1; 
      if (doy < baseDay)
	return offs;
      else
	return (doy - baseDay)/7 + 1 + offs;
  }


  double date_time_c::julianDay() const
  {
    return toJulianDay(m_utcTime);
  }


  date_time_c date_time_c::operator + (const time_span_c& span) const
  {
    return date_time_c(m_utcTime, span.totalMicroseconds());
  }


  date_time_c date_time_c::operator - (const time_span_c& span) const
  {
    return date_time_c(m_utcTime, -span.totalMicroseconds());
  }


  time_span_c date_time_c::operator - (const date_time_c& dateTime) const
  {
    return time_span_c((m_utcTime - dateTime.m_utcTime)/10);
  }


  date_time_c& date_time_c::operator += (const time_span_c& span)
  {
    m_utcTime += span.totalMicroseconds()*10;
    computeGregorian(julianDay());
    computeDaytime();
    return *this;
  }


  date_time_c& date_time_c::operator -= (const time_span_c& span)
  {
    m_utcTime -= span.totalMicroseconds()*10;
    computeGregorian(julianDay());
    computeDaytime();
    return *this;
  }


  void date_time_c::makeUTC(int tzd)
  {
    operator -= (time_span_c(((time_stamp_c::TimeDiff) tzd)*time_span_c::SECONDS));
  }

	
  void date_time_c::makeLocal(int tzd)
  {
    operator += (time_span_c(((time_stamp_c::TimeDiff) tzd)*time_span_c::SECONDS));
  }


  double date_time_c::toJulianDay(int year, int month, int day, int hour, int minute, int second, int millisecond, int microsecond)
  {
    // lookup table for (153*month - 457)/5 - note that 3 <= month <= 14.
    static int lookup[] = {-91, -60, -30, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337};
 
    // day to double
    double dday = double(day) + ((double((hour*60 + minute)*60 + second)*1000 + millisecond)*1000 + microsecond)/86400000000.0;
    if (month < 3)
      {
	month += 12;
	--year;
      }
    double dyear = double(year);
    return dday + lookup[month] + 365*year + std::floor(dyear/4) - std::floor(dyear/100) + std::floor(dyear/400) + 1721118.5;
  }


  void date_time_c::checkLimit(short& lower, short& higher, short limit)
  {
    if (lower >= limit)
      {
	higher += short(lower / limit);
	lower   = short(lower % limit);
      }
  }


  void date_time_c::normalize()
  {
    checkLimit(m_microsecond, m_millisecond, 1000);
    checkLimit(m_millisecond, m_second, 1000);
    checkLimit(m_second, m_minute, 60);
    checkLimit(m_minute, m_hour, 60);
    checkLimit(m_hour, m_day, 24);

    if (m_day > daysOfMonth(m_year, m_month))
      {
	m_day -= daysOfMonth(m_year, m_month);
	if (++m_month > 12)
	  {
	    ++m_year;
	    m_month -= 12;
	  }
      }
  }


  void date_time_c::computeGregorian(double julianDay)
  {
    double z    = std::floor(julianDay - 1721118.5);
    double r    = julianDay - 1721118.5 - z;
    double g    = z - 0.25;
    double a    = std::floor(g / 36524.25);
    double b    = a - std::floor(a/4);
    m_year       = short(std::floor((b + g)/365.25));
    double c    = b + z - std::floor(365.25*m_year);
    m_month      = short(std::floor((5*c + 456)/153));
    double dday = c - std::floor((153.0*m_month - 457)/5) + r;
    m_day        = short(dday);
    if (m_month > 12)
      {
	++m_year;
	m_month -= 12;
      }
    r      *= 24;
    m_hour   = short(std::floor(r));
    r      -= std::floor(r);
    r      *= 60;
    m_minute = short(std::floor(r));
    r      -= std::floor(r);
    r      *= 60;
    m_second = short(std::floor(r));
    r      -= std::floor(r);
    r      *= 1000;
    m_millisecond = short(std::floor(r));
    r      -= std::floor(r);
    r      *= 1000;
    m_microsecond = short(r + 0.5);

    normalize();

    poco_assert_dbg (m_month >= 1 && m_month <= 12);
    poco_assert_dbg (m_day >= 1 && m_day <= daysOfMonth(m_year, m_month));
    poco_assert_dbg (m_hour >= 0 && m_hour <= 23);
    poco_assert_dbg (m_minute >= 0 && m_minute <= 59);
    poco_assert_dbg (m_second >= 0 && m_second <= 59);
    poco_assert_dbg (m_millisecond >= 0 && m_millisecond <= 999);
    poco_assert_dbg (m_microsecond >= 0 && m_microsecond <= 999);
  }


  void date_time_c::computeDaytime()
  {
    time_span_c span(m_utcTime/10);
    m_hour        = span.hours();
    m_minute      = span.minutes();
    m_second      = span.seconds();
    m_millisecond = span.milliseconds();
    m_microsecond = span.microseconds();
  }

  time_stamp_c date_time_c::timestamp() const
  {
    return time_stamp_c::fromUtcTime(m_utcTime);
  }


  time_stamp_c::utc_time_val_t date_time_c::utcTime() const
  {
    return m_utcTime;
  }


  int date_time_c::year() const
  {
    return m_year;
  }

	
  int date_time_c::month() const
  {
    return m_month;
  }

	
  int date_time_c::day() const
  {
    return m_day;
  }

	
  int date_time_c::hour() const
  {
    return m_hour;
  }


  int date_time_c::hourAMPM() const
  {
    if (m_hour < 1)
      return 12;
    else if (m_hour > 12)
      return m_hour - 12;
    else
      return m_hour;
  }


  bool date_time_c::isAM() const
  {
    return m_hour < 12;
  }


  bool date_time_c::isPM() const
  {
    return m_hour >= 12;
  }

	
  int date_time_c::minute() const
  {
    return m_minute;
  }

	
  int date_time_c::second() const
  {
    return m_second;
  }

	
  int date_time_c::millisecond() const
  {
    return m_millisecond;
  }


  int date_time_c::microsecond() const
  {
    return m_microsecond;
  }


  bool date_time_c::operator == (const date_time_c& dateTime) const
  {
    return m_utcTime == dateTime.m_utcTime;
  }


  bool date_time_c::operator != (const date_time_c& dateTime) const	
  {
    return m_utcTime != dateTime.m_utcTime;
  }


  bool date_time_c::operator <  (const date_time_c& dateTime) const	
  {
    return m_utcTime < dateTime.m_utcTime;
  }


  bool date_time_c::operator <= (const date_time_c& dateTime) const
  {
    return m_utcTime <= dateTime.m_utcTime;
  }


  bool date_time_c::operator >  (const date_time_c& dateTime) const
  {
    return m_utcTime > dateTime.m_utcTime;
  }


  bool date_time_c::operator >= (const date_time_c& dateTime) const	
  {
    return m_utcTime >= dateTime.m_utcTime;
  }


  bool date_time_c::isLeapYear(int year)
  {
    return (year % 4) == 0 && ((year % 100) != 0 || (year % 400) == 0);
  }


  void swap(date_time_c& d1, date_time_c& d2)
  {
    d1.swap(d2);
  }

} // namespace Poco
