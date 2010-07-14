#include "abl/date_time/date_time_format.hpp"

#define DTF_MEMBER const std::string date_time_format_c
namespace abl 
{
  DTF_MEMBER::ISO8601_FORMAT  = "%Y-%m-%dT%H:%M:%S%z";
  DTF_MEMBER::RFC822_FORMAT   = "%w, %e %b %y %H:%M:%S %Z";
  DTF_MEMBER::RFC1123_FORMAT  = "%w, %e %b %Y %H:%M:%S %Z";
  DTF_MEMBER::HTTP_FORMAT     = "%w, %d %b %Y %H:%M:%S %Z";
  DTF_MEMBER::RFC850_FORMAT   = "%W, %e-%b-%y %H:%M:%S %Z";
  DTF_MEMBER::RFC1036_FORMAT  = "%W, %e %b %y %H:%M:%S %Z";
  DTF_MEMBER::ASCTIME_FORMAT  = "%w %b %f %H:%M:%S %Y";
  DTF_MEMBER::SORTABLE_FORMAT = "%Y-%m-%d %H:%M:%S";
  // ----------------------------------------------------------------
  DTF_MEMBER::WEEKDAY_NAMES[] =
    {
      "Sunday",
      "Monday",
      "Tuesday",
      "Wednesday",
      "Thursday",
      "Friday",
      "Saturday"
    };
  // ----------------------------------------------------------------
  DTF_MEMBER::MONTH_NAMES[] =
    {
      "January",
      "February",
      "March",
      "April",
      "May",
      "June",
      "July",
      "August",
      "September",
      "October",
      "November",
      "December"
    };
} 
