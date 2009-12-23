#include "abl/date_time/date_time_parser.hpp"
#include "abl/date_time/date_time_format.hpp"
#include "abl/date_time/date_time.hpp"
#include "abl/exception.hpp"

#include <cctype>


namespace abl 
{


#define SKIP_JUNK()					\
  while  (it != end && !std::isdigit (*it)) ++it


#define PARSE_NUMBER(var)						\
  while  (it != end && std::isdigit (*it)) var = var*10 +  ( (*it++) - '0')


#define PARSE_NUMBER_N(var, n)						\
  { int i = 0; while  (i++ < n && it != end && std::isdigit (*it)) var = var*10 +  ( (*it++) - '0'); }

  // -----------------------------------------------------------------
  void date_time_parser_c::parse (const std::string& fmt, 
				  const std::string& str, 
				  date_time_c& dateTime, 
				  int& timeZoneDifferential)
  {
    int year   = 0;
    int month  = 0;
    int day    = 0;
    int hour   = 0;
    int minute = 0;
    int second = 0;
    int millis = 0;
    int micros = 0;
    int tzd    = 0;

    std::string::const_iterator it   = str.begin ();
    std::string::const_iterator end  = str.end ();
    std::string::const_iterator itf  = fmt.begin ();
    std::string::const_iterator endf = fmt.end ();

    while  (itf != endf && it != end)
      {
	if  (*itf == '%')
	  {
	    if  (++itf != endf)
	      {
		switch  (*itf)
		  {
		  case 'w':
		  case 'W':
		    while  (it != end && std::isspace (*it)) ++it;
		    while  (it != end && std::isalpha (*it)) ++it;
		    break;
		  case 'b':
		  case 'B':
		    month = parse_month (it, end);
		    break;
		  case 'd':
		  case 'e':
		  case 'f':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (day, 2);
		    break;
		  case 'm':
		  case 'n':
		  case 'o':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (month, 2);
		    break;					 
		  case 'y':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (year, 2);
		    if  (year >= 69) 
		      year += 1900;
		    else
		      year += 2000;
		    break;
		  case 'Y':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (year, 4);
		    break;
		  case 'r':
		    SKIP_JUNK ();
		    PARSE_NUMBER (year);
		    if  (year < 1000)
		      {
			if  (year >= 69) 
			  year += 1900;
			else
			  year += 2000;
		      }
		    break;
		  case 'H':
		  case 'h':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (hour, 2);
		    break;
		  case 'a':
		  case 'A':
		    hour = _parse_am_pm (it, end, hour);
		    break;
		  case 'M':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (minute, 2);
		    break;
		  case 'S':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (second, 2);
		    break;
		  case 'i':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (millis, 3);
		    break;
		  case 'c':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (millis, 1);
		    millis *= 100;
		    break;
		  case 'F':
		    SKIP_JUNK ();
		    PARSE_NUMBER_N (millis, 3);
		    PARSE_NUMBER_N (micros, 3);
		    break;
		  case 'z':
		  case 'Z':
		    tzd = _parse_tzd (it, end);
		    break;
		  }
		++itf;
	      }
	  }
	else ++itf;
      }
    if  (month == 0) month = 1;
    if  (day == 0) day = 1;
    if  (date_time_c::is_valid (year, month, day, hour, minute, second, millis, micros))
      dateTime.assign (year, month, day, hour, minute, second, millis, micros);
    else 
      throw syntax_exception_c ("date/time component out of range");
    timeZoneDifferential = tzd;
  }
  // -----------------------------------------------------------------
  date_time_c date_time_parser_c::parse (const std::string& fmt, 
					 const std::string& str, 
					 int& timeZoneDifferential)
  {
    date_time_c result;
    parse (fmt, str, result, timeZoneDifferential);
    return result;
  }
  // -----------------------------------------------------------------
  bool date_time_parser_c::try_parse (const std::string& fmt, 
				      const std::string& str, 
				      date_time_c& dateTime, 
				      int& timeZoneDifferential)
  {
    try
      {
	parse (fmt, str, dateTime, timeZoneDifferential);
      }
    catch  (exception_c&)
      {
	return false;
      }
    return true;
  }
  // -----------------------------------------------------------------

  void date_time_parser_c::parse (const std::string& str, 
				  date_time_c& dateTime, 
				  int& timeZoneDifferential)
  {
    if  (!try_parse (str, dateTime, timeZoneDifferential))
      throw syntax_exception_c ("Unsupported or invalid date/time format");
  }
  // -----------------------------------------------------------------
	
  date_time_c date_time_parser_c::parse (const std::string& str, 
					 int& timeZoneDifferential)
  {
    date_time_c result;
    if  (try_parse (str, result, timeZoneDifferential))
      return result;
    else
      throw syntax_exception_c ("Unsupported or invalid date/time format");
  }
  // -----------------------------------------------------------------
	
  bool date_time_parser_c::try_parse (const std::string& str, 
				      date_time_c& dateTime, 
				      int& timeZoneDifferential)
  {
    if  (str.length () < 4) return false;
	
    if  (str[3] == ',')
      return try_parse ("%w, %e %b %r %H:%M:%S %Z", str, dateTime, timeZoneDifferential);
    else if  (str[3] == ' ')
      return try_parse (date_time_format_c::ASCTIME_FORMAT, str, dateTime, timeZoneDifferential);
    else if  (str.find (',') != std::string::npos)
      return try_parse ("%W, %e %b %r %H:%M:%S %Z", str, dateTime, timeZoneDifferential);
    else if  (std::isdigit (str[0]))
      {
	if  (str.find (' ') != std::string::npos || str.length () == 10)
	  return try_parse (date_time_format_c::SORTABLE_FORMAT, str, dateTime, timeZoneDifferential);
	else
	  return try_parse (date_time_format_c::ISO8601_FORMAT, str, dateTime, timeZoneDifferential);
      }
    else return false;
  }
  // -----------------------------------------------------------------
  int date_time_parser_c::_parse_tzd (std::string::const_iterator& it, 
				      const std::string::const_iterator& end)
  {
    struct Zone
    {
      const char* designator;
      int         timeZoneDifferential;
    };

    static Zone zones[] =
      {
	{"Z",           0},
	{"UT",          0},
	{"GMT",         0},
	{"BST",    1*3600},
	{"IST",    1*3600},
	{"WET",         0},
	{"WEST",   1*3600},
	{"CET",    1*3600},
	{"CEST",   2*3600},
	{"EET",    2*3600},
	{"EEST",   3*3600},
	{"MSK",    3*3600},
	{"MSD",    4*3600},
	{"NST",   -3*3600-1800},
	{"NDT",   -2*3600-1800},
	{"AST",   -4*3600},
	{"ADT",   -3*3600},
	{"EST",   -5*3600},
	{"EDT",   -4*3600},
	{"CST",   -6*3600},
	{"CDT",   -5*3600},
	{"MST",   -7*3600},
	{"MDT",   -6*3600},
	{"PST",   -8*3600},
	{"PDT",   -7*3600},
	{"AKST",  -9*3600},
	{"AKDT",  -8*3600},
	{"HST",  -10*3600},
	{"AEST",  10*3600},
	{"AEDT",  11*3600},
	{"ACST",   9*3600+1800},
	{"ACDT",  10*3600+1800},
	{"AWST",   8*3600},
	{"AWDT",   9*3600}
      };

    while  (it != end && std::isspace (*it)) ++it;
    if  (it != end)
      {
	if  (std::isalpha (*it))
	  {
	    std::string designator;
	    designator += *it++;
	    if  (it != end && std::isalpha (*it)) designator += *it++;
	    if  (it != end && std::isalpha (*it)) designator += *it++;
	    if  (it != end && std::isalpha (*it)) designator += *it++;
	    for  (unsigned i = 0; i < sizeof (zones)/sizeof (Zone); ++i)
	      {
		if  (designator == zones[i].designator)
		  return zones[i].timeZoneDifferential;
	      }
	  }
	else if  (*it == '+' || *it == '-')
	  {
	    int sign = *it == '+' ? 1 : -1;
	    ++it;
	    int hours = 0;
	    PARSE_NUMBER_N (hours, 2);
	    if  (it != end && *it == ':') ++it;
	    int minutes = 0;
	    PARSE_NUMBER_N (minutes, 2);
	    return sign* (hours*3600 + minutes*60);
	  }
      }
    return 0;
  }
  // -----------------------------------------------------------------
  int date_time_parser_c::parse_month (std::string::const_iterator& it, 
				       const std::string::const_iterator& end)
  {
    std::string month;
    while  ( (it != end && std::isspace (*it)) || std::ispunct (*it)) ++it;
    bool isFirst = true;
    while  (it != end && std::isalpha (*it)) 
      {
	char ch =  (*it++);
	if  (isFirst) { month += std::toupper (ch); isFirst = false; }
	else month += std::tolower (ch);
      }
    if  (month.length () < 3) 
      throw syntax_exception_c ("Month name must be at least three characters long", month);
    for  (int i = 0; i < 12; ++i) 
      {
	if  (date_time_format_c::MONTH_NAMES[i].find (month) == 0)
	  return i + 1;
      }
    throw syntax_exception_c ("Not a valid month name", month);
  }
  // -----------------------------------------------------------------
  int 
  date_time_parser_c::parse_day_of_week (std::string::const_iterator& it, 
					 const std::string::const_iterator& end)
  {
    std::string dow;
    while  ( (it != end && std::isspace (*it)) || std::ispunct (*it)) ++it;
    bool isFirst = true;
    while  (it != end && std::isalpha (*it)) 
      {
	char ch =  (*it++);
	if  (isFirst) { dow += std::toupper (ch); isFirst = false; }
	else dow += std::tolower (ch);
      }
    if  (dow.length () < 3) throw syntax_exception_c ("Weekday name must be at least three characters long", dow);
    for  (int i = 0; i < 7; ++i) 
      {
	if  (date_time_format_c::WEEKDAY_NAMES[i].find (dow) == 0)
	  return i;
      }
    throw syntax_exception_c ("Not a valid weekday name", dow);
  }
  // -----------------------------------------------------------------
  int date_time_parser_c::_parse_am_pm (std::string::const_iterator& it, 
					const std::string::const_iterator& end, 
					int hour)
  {
    std::string ampm;
    while  ( (it != end && std::isspace (*it)) || std::ispunct (*it)) ++it;
    while  (it != end && std::isalpha (*it)) 
      {
	char ch =  (*it++);
	ampm += std::toupper (ch);
      }
    if  (ampm == "AM")
      {
	if  (hour == 12)
	  return 0;
	else
	  return hour;
      }
    else if  (ampm == "PM")
      {
	if  (hour < 12)
	  return hour + 12;
	else
	  return hour;
      }
    else throw syntax_exception_c ("Not a valid AM/PM designator", ampm);
  }

} // namespace Poco
