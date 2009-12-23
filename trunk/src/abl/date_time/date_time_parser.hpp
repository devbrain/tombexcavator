/**
 * @file   date_time_parser.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Wed Dec 23 16:21:12 2009
 * 
 * @brief  Definition of the date_time_parser_c class.
 */

#ifndef __ABL_DATE_TIME_PARSER_HPP__
#define __ABL_DATE_TIME_PARSER_HPP__


#include "abl/abl_export.h"
#include "abl/date_time/date_time.hpp"
#include <string>

namespace abl 
{
  /**
   * This class provides a method for parsing dates and times
   * from strings. All parsing methods do their best to
   * parse a meaningful result, even from malformed input
   * strings.
   *
   * The returned date_time_c will always contain a time in the same
   * timezone as the time in the string. Call date_time_c::makeUTC ()
   * with the timeZoneDifferential returned by parse () to convert
   * the date_time_c to UTC.
   *
   * Note: When parsing a time in 12-hour  (AM/PM) format, the hour
   *  (%h) must be parsed before the AM/PM designator  (%a, %A),
   * otherwise the AM/PM designator will be ignored.
   *
   * See the date_time_formatter_c class for a list of supported 
   * format specifiers.
   * In addition to the format specifiers supported by DateTimeFormatter, an
   * additional specifier is supported: %r will parse a year given by either
   * two or four digits. Years 69-00 are interpreted in the 20th century
   *  (1969-2000), years 01-68 in the 21th century  (2001-2068).
   */
  class ABL_API date_time_parser_c
  {
  public:
    /**
     * Parses a date and time in the given format from the given string.
     * Throws a SyntaxException if the string cannot be successfully parsed.
     * Please see DateTimeFormatter::format () for a description of the format 
     * string.
     * Class DateTimeFormat defines format strings for various standard 
     * date/time formats.
     */
    static void parse (const std::string& fmt, const std::string& str, 
		       date_time_c& dateTime, int& timeZoneDifferential);
    /**
     * Parses a date and time in the given format from the given string.
     * Throws a SyntaxException if the string cannot be successfully parsed.
     * Please see date_time_formatter_c::format () for a description of the 
     * format string.
     * Class DateTimeFormat defines format strings for various standard 
     * date/time formats.
     */			
    static date_time_c parse (const std::string& fmt, const std::string& str, 
			   int& timeZoneDifferential);
    /**
     * Parses a date and time in the given format from the given string.
     * Returns true if the string has been successfully parsed, 
     * false otherwise.
     * Please see date_time_formatter_c::format () for a description of the 
     * format string.
     * Class DateTimeFormat defines format strings for various 
     * standard date/time formats.
     */	
    static bool try_parse (const std::string& fmt, const std::string& str, 
			   date_time_c& dateTime, int& timeZoneDifferential);
    /**
     * Parses a date and time from the given dateTime string. 
     * Before parsing, the method
     * examines the dateTime string for a known date/time format.
     * Throws a SyntaxException if the string cannot be successfully parsed.
     * Please see DateTimeFormatter::format () for a description of the 
     * format string.
     * Class DateTimeFormat defines format strings for various standard 
     * date/time formats.
     */
    static void parse (const std::string& str, date_time_c& dateTime, 
		       int& timeZoneDifferential);
    /**
     * Parses a date and time from the given dateTime string. 
     * Before parsing, the method
     * examines the dateTime string for a known date/time format.
     * Please see DateTimeFormatter::format () for a description of the 
     * format string.
     * Class DateTimeFormat defines format strings for various standard 
     * date/time formats.
     */			
    static date_time_c parse (const std::string& str, 
			      int& timeZoneDifferential);
    /**
     * Parses a date and time from the given dateTime string. 
     * Before parsing, the method
     * examines the dateTime string for a known date/time format.
     * Please see DateTimeFormatter::format () for a description of the 
     * format string.
     * Class DateTimeFormat defines format strings for various standard 
     * date/time formats.
     */
    static bool try_parse (const std::string& str, date_time_c& dateTime, 
			   int& timeZoneDifferential);
    /**
     * Tries to interpret the given range as a month name. 
     * The range must be at least three characters long. 
     * Returns the month number  (1 .. 12) if the month name is valid. 
     * Otherwise throws a SyntaxException.		
     */
    static int parse_month (std::string::const_iterator& it, 
			   const std::string::const_iterator& end);
    /**
     * Tries to interpret the given range as a weekday name. 
     * The range must be at least
     * three characters long. 
     * Returns the weekday number  (0 .. 6, where 0 = Synday, 1 = Monday, etc.)
     * if the weekday name is valid. Otherwise throws a SyntaxException.
     */
    static int parse_day_of_week (std::string::const_iterator& it, 
				  const std::string::const_iterator& end);

		
  protected:
    static int _parse_tzd (std::string::const_iterator& it, 
			   const std::string::const_iterator& end);
    static int _parse_am_pm (std::string::const_iterator& it, 
			     const std::string::const_iterator& end, int hour);
  };


} 


#endif 
