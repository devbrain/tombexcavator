#include "abl/date_time/date_time_formatter.hpp"
#include "abl/date_time/date_time_format.hpp"
#include "abl/date_time/time_stamp.hpp"
#include "abl/number_formatter.hpp"


namespace abl 
{
  void date_time_formatter_c::append (std::string& str, const date_time_c& dateTime, 
				      const std::string& fmt, int timeZoneDifferential)
  {
    std::string::const_iterator it  = fmt.begin ();
    std::string::const_iterator end = fmt.end ();
    while  (it != end)
      {
	if  (*it == '%')
	  {
	    if  (++it != end)
	      {
		switch  (*it)
		  {
		  case 'w': 
		    str.append (date_time_format_c::WEEKDAY_NAMES [dateTime.day_of_week ()], 0, 3); 
		    break;
		  case 'W': 
		    str.append (date_time_format_c::WEEKDAY_NAMES [dateTime.day_of_week ()]); 
		    break;
		  case 'b': 
		    str.append (date_time_format_c::MONTH_NAMES [dateTime.month () - 1], 0, 3); 
		    break;
		  case 'B': 
		    str.append (date_time_format_c::MONTH_NAMES [dateTime.month () - 1]); 
		    break;
		  case 'd': 
		    number_formatter_c::append0 (str, dateTime.day (), 2); 
		    break;
		  case 'e': 
		    number_formatter_c::append (str, dateTime.day ()); 
		    break;
		  case 'f': 
		    number_formatter_c::append (str, dateTime.day (), 2); 
		    break;
		  case 'm': 
		    number_formatter_c::append0 (str, dateTime.month (), 2); 
		    break;
		  case 'n': 
		    number_formatter_c::append (str, dateTime.month ()); 
		    break;
		  case 'o': 
		    number_formatter_c::append (str, dateTime.month (), 2); 
		    break;
		  case 'y': 
		    number_formatter_c::append0 (str, dateTime.year () % 100, 2); 
		    break;
		  case 'Y': 
		    number_formatter_c::append0 (str, dateTime.year (), 4); 
		    break;
		  case 'H': 
		    number_formatter_c::append0 (str, dateTime.hour (), 2); 
		    break;
		  case 'h': 
		    number_formatter_c::append0 (str, dateTime.hour_am_pm (), 2); 
		    break;
		  case 'a': 
		    str.append (dateTime.is_am () ? "am" : "pm"); 
		    break;
		  case 'A': 
		    str.append (dateTime.is_pm () ? "AM" : "PM"); 
		    break;
		  case 'M': 
		    number_formatter_c::append0 (str, dateTime.minute (), 2); 
		    break;
		  case 'S': 
		    number_formatter_c::append0 (str, dateTime.second (), 2); 
		    break;
		  case 'i': 
		    number_formatter_c::append0 (str, dateTime.millisecond (), 3); 
		    break;
		  case 'c': 
		    number_formatter_c::append (str, dateTime.millisecond ()/100); 
		    break;
		  case 'F': 
		    number_formatter_c::append0 (str, 
						 dateTime.millisecond ()*1000 + dateTime.microsecond (), 6); 
		    break;
		  case 'z': 
		    tzd_iso (str, timeZoneDifferential); 
		    break;
		  case 'Z': 
		    tzd_rfc (str, timeZoneDifferential); 
		    break;
		  default:  str += *it;
		  }
		++it;
	      }
	  }
	else str += *it++;
      }
  }
  // -----------------------------------------------------------------------------------
  void date_time_formatter_c::append (std::string& str, const 
				      time_span_c& timespan, 
				      const std::string& fmt)
  {
    std::string::const_iterator it  = fmt.begin ();
    std::string::const_iterator end = fmt.end ();
    while  (it != end)
      {
	if  (*it == '%')
	  {
	    if  (++it != end)
	      {
		switch  (*it)
		  {
		  case 'd': 
		    number_formatter_c::append (str, timespan.days ()); 
		    break;
		  case 'H': 
		    number_formatter_c::append0 (str, timespan.hours (), 2); 
		    break;
		  case 'h': 
		    number_formatter_c::append (str, timespan.total_hours ()); 
		    break;
		  case 'M': 
		    number_formatter_c::append0 (str, timespan.minutes (), 2); 
		    break;
		  case 'm': 
		    number_formatter_c::append (str, timespan.total_minutes ()); 
		    break;
		  case 'S': 
		    number_formatter_c::append0 (str, timespan.seconds (), 2); 
		    break;
		  case 's': 
		    number_formatter_c::append (str, timespan.total_seconds ()); 
		    break;
		  case 'i': 
		    number_formatter_c::append0 (str, timespan.milliseconds (), 3); 
		    break;
		  case 'c': 
		    number_formatter_c::append (str, timespan.milliseconds ()/100); 
		    break;
		  case 'F': 
		    number_formatter_c::append0 (str, 
						 timespan.milliseconds ()*1000 + timespan.microseconds (), 6);

		    break;
		  default:  str += *it;
		  }
		++it;
	      }
	  }
	else str += *it++;
      }
  }
  // -----------------------------------------------------------------------------------
  void date_time_formatter_c::tzd_iso (std::string& str, int timeZoneDifferential)
  {
    if  (timeZoneDifferential != UTC)
      {
	if  (timeZoneDifferential >= 0)
	  {
	    str += '+';
	    number_formatter_c::append0 (str, timeZoneDifferential/3600, 2);
	    str += ':';
	    number_formatter_c::append0 (str,  (timeZoneDifferential%3600)/60, 2);
	  }
	else
	  {
	    str += '-';
	    number_formatter_c::append0 (str, -timeZoneDifferential/3600, 2);
	    str += ':';
	    number_formatter_c::append0 (str,  (-timeZoneDifferential%3600)/60, 2);
	  }
      }
    else str += 'Z';
  }
  // -----------------------------------------------------------------------------------
  void date_time_formatter_c::tzd_rfc (std::string& str, int timeZoneDifferential)
  {
    if  (timeZoneDifferential != UTC)
      {
	if  (timeZoneDifferential >= 0)
	  {
	    str += '+';
	    number_formatter_c::append0 (str, timeZoneDifferential/3600, 2);
	    number_formatter_c::append0 (str,  (timeZoneDifferential%3600)/60, 2);
	  }
	else
	  {
	    str += '-';
	    number_formatter_c::append0 (str, -timeZoneDifferential/3600, 2);
	    number_formatter_c::append0 (str,  (-timeZoneDifferential%3600)/60, 2);
	  }		
      }
    else str += "GMT";
  }
  // -----------------------------------------------------------------------------------
  std::string date_time_formatter_c::format (const time_stamp_c& timestamp, const std::string& fmt, 
					     int timeZoneDifferential)
  {
    date_time_c dateTime (timestamp);
    return format (dateTime, fmt, timeZoneDifferential);
  }
  // -----------------------------------------------------------------------------------
  std::string date_time_formatter_c::format (const date_time_c& dateTime, const std::string& fmt, 
					     int timeZoneDifferential)
  {
    std::string result;
    result.reserve (64);
    append (result, dateTime, fmt, timeZoneDifferential);
    return result;
  }
  // -----------------------------------------------------------------------------------
  std::string date_time_formatter_c::format (const local_date_time_c& dateTime, const std::string& fmt)
  {
    return format (dateTime.m_date_time, fmt, dateTime.m_tzd);
  }
  // -----------------------------------------------------------------------------------
  std::string date_time_formatter_c::format (const time_span_c& timespan, 
					     const std::string& fmt)
  {
    std::string result;
    result.reserve (32);
    append (result, timespan, fmt);
    return result;
  }
  // -----------------------------------------------------------------------------------
  void date_time_formatter_c::append (std::string& str, const time_stamp_c& timestamp, 
				      const std::string& fmt, int timeZoneDifferential)
  {
    date_time_c dateTime (timestamp);
    append (str, dateTime, fmt, timeZoneDifferential);
  }
  // -----------------------------------------------------------------------------------
  std::string date_time_formatter_c::tzd_iso (int timeZoneDifferential)
  {
    std::string result;
    result.reserve (8);
    tzd_iso (result, timeZoneDifferential);
    return result;
  }
  // -----------------------------------------------------------------------------------
  std::string date_time_formatter_c::tzd_rfc (int timeZoneDifferential)
  {
    std::string result;
    result.reserve (8);
    tzd_rfc (result, timeZoneDifferential);
    return result;
  }


} 
