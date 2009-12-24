/**
 * @file   date_time_formatter.hpp
 * @author igor <igorgu@il.ibm.com>
 * @date   Thu Dec 24 23:06:40 2009
 * 
 * @brief  Definition of the date_time_formatter_c class.
 * 
 */

#ifndef __ABL_DATE_TIME_FORMATTER_HPP__
#define __ABL_DATE_TIME_FORMATTER_HPP__

#include "abl/abl_export.h"
#include "abl/date_time/date_time.hpp"
#include "abl/date_time/local_date_time.hpp"

#include <string>

namespace abl 
{
  class time_stamp_c;
  class time_span_c;
  /**
   * This class converts dates and times into strings, supporting a  
   * variety of standard and custom formats.
   *
   * There are two kind of static member functions:
   *    * format* functions return a std::string containing
   *      the formatted value.
   *    * append* functions append the formatted value to
   *      an existing string.
   */
  class ABL_API date_time_formatter_c
  {
  public:
    enum
      {
	UTC = 0xFFFF /**< Special value for timeZoneDifferential denoting UTC. */
      };
    /**
     * Formats the given timestamp according to the given format.
     * The format string is used as a template to format the date and
     * is copied character by character except for the following special characters,
     * which are replaced by the corresponding value.
     *
     *   * %w - abbreviated weekday  (Mon, Tue, ...)
     *   * %W - full weekday  (Monday, Tuesday, ...)
     *   * %b - abbreviated month  (Jan, Feb, ...)
     *   * %B - full month  (January, February, ...)
     *   * %d - zero-padded day of month  (01 .. 31)
     *   * %e - day of month  (1 .. 31)
     *   * %f - space-padded day of month  ( 1 .. 31)
     *   * %m - zero-padded month  (01 .. 12)
     *   * %n - month  (1 .. 12)
     *   * %o - space-padded month  ( 1 .. 12)
     *   * %y - year without century  (70)
     *   * %Y - year with century  (1970)
     *   * %H - hour  (00 .. 23)
     *   * %h - hour  (00 .. 12)
     *   * %a - am/pm
     *   * %A - AM/PM
     *   * %M - minute  (00 .. 59)
     *   * %S - second  (00 .. 59)
     *   * %i - millisecond  (000 .. 999)
     *   * %c - centisecond  (0 .. 9)
     *   * %F - fractional seconds/microseconds  (000000 - 999999)
     *   * %z - time zone differential in ISO 8601 format  (Z or +NN.NN).
     *   * %Z - time zone differential in RFC format  (GMT or +NNNN)
     *   * %% - percent sign
     *
     * Class date_time_cFormat defines format strings for various standard date/time formats.
     */
    static std::string format (const time_stamp_c& timestamp, const std::string& fmt, 
			       int timeZoneDifferential = UTC);
    /**
     * Formats the given date and time according to the given format.
     * See format (const time_stamp_c&, const std::string&, int) for more information.
     */
    static std::string format (const date_time_c& dateTime, const std::string& fmt, 
			       int timeZoneDifferential = UTC);
    /**
     * Formats the given local date and time according to the given format.
     * See format (const time_stamp_c&, const std::string&, int) for more information.
     */
    static std::string format (const local_date_time_c& dateTime, const std::string& fmt);
    /**
     * Formats the given timespan according to the given format.
     * The format string is used as a template to format the date and
     * is copied character by character except for the following special characters,
     * which are replaced by the corresponding value.
     *
     *   * %d - days
     *   * %H - hours	  (00 .. 23)
     *   * %h - total hours  (0 .. n)
     *   * %M - minutes  (00 .. 59)
     *   * %m - total minutes  (0 .. n)
     *   * %S - seconds  (00 .. 59)
     *   * %s - total seconds  (0 .. n)
     *   * %i - milliseconds  (000 .. 999)
     *   * %c - centisecond  (0 .. 9)
     *   * %F - fractional seconds/microseconds  (000000 - 999999)
     *   * %% - percent sign
     */
    static std::string format (const time_span_c& timespan, 
			       const std::string& fmt = "%dd %H:%M:%S.%i");
    /**
     * Formats the given timestamp according to the given format and appends it to str.
     *
     * See format () for documentation of the formatting string.
     */
    static void append (std::string& str, const time_stamp_c& timestamp, 
			const std::string& fmt, int timeZoneDifferential = UTC);
    /**
     * Formats the given date and time according to the given format and appends it to str.
     *
     * See format () for documentation of the formatting string.
     */
    static void append (std::string& str, const date_time_c& dateTime, 
			const std::string& fmt, int timeZoneDifferential = UTC);
    /**
     * Formats the given local date and time according to the given format and appends it to str.
     *
     * See format () for documentation of the formatting string.
     */
    static void append (std::string& str, const local_date_time_c& dateTime, 
			const std::string& fmt);
    /**
     * Formats the given timespan according to the given format and appends it to str.
     *
     * See format () for documentation of the formatting string.
     */
    static void append (std::string& str, const time_span_c& timespan, 
			const std::string& fmt = "%dd %H:%M:%S.%i");
    /**
     * Formats the given timezone differential in ISO format.
     * If timeZoneDifferential is UTC, "Z" is returned,
     * otherwise, +HH.MM  (or -HH.MM) is returned.
     */
    static std::string tzd_iso (int timeZoneDifferential);
    /**
     * Formats the given timezone differential in RFC format.
     * If timeZoneDifferential is UTC, "GMT" is returned,
     * otherwise ++HHMM  (or -HHMM) is returned.		
     */
    static std::string tzd_rfc (int timeZoneDifferential);
    /**
     * Formats the given timezone differential in ISO format
     * and appends it to the given string.
     * If timeZoneDifferential is UTC, "Z" is returned,
     * otherwise, +HH.MM  (or -HH.MM) is returned.
     */
    static void tzd_iso (std::string& str, int timeZoneDifferential);
    /**
     * Formats the given timezone differential in RFC format
     * and appends it to the given string.
     * If timeZoneDifferential is UTC, "GMT" is returned,
     * otherwise ++HHMM  (or -HHMM) is returned.
     */
    static void tzd_rfc (std::string& str, int timeZoneDifferential);

  };
} 
#endif 
