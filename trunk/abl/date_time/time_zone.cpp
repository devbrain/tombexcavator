#include "abl/date_time/time_zone.hpp"

#if defined(_WIN32)
#include "abl/string/unicode_converter.hpp"
#include <windows.h>
#else
#endif

#include <ctime>

namespace abl 
{
  int time_zone_c::tzd ()
  {
    return utc_offset () + dst ();
  }
// -----------------------------------------------------------------
#if defined (_WIN32)
  int time_zone_c::utc_offset ()
  {
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation (&tzInfo);
    return -tzInfo.Bias*60;
  }
  // -----------------------------------------------------------------
  int time_zone_c::dst ()
  {
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation (&tzInfo);
    return dstFlag == TIME_ZONE_ID_DAYLIGHT ? -tzInfo.DaylightBias*60 : 0;
  }
  // -----------------------------------------------------------------
  bool time_zone_c::is_dst (const time_stamp_c& timestamp)
  {
    std::time_t time = timestamp.epoch_time ();
    struct std::tm* tms = std::localtime (&time);
    return tms->tm_isdst > 0;
  }
  // -----------------------------------------------------------------
  std::string time_zone_c::name ()
  {
    std::string result;
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation (&tzInfo);
    WCHAR* ptr = dstFlag == TIME_ZONE_ID_DAYLIGHT ? tzInfo.DaylightName : tzInfo.StandardName;

    unicode_converter_c::to_utf8 (ptr, result);
    return result;
  }
  // -----------------------------------------------------------------
  std::string time_zone_c::standard_name ()
  {
    std::string result;
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation (&tzInfo);
    WCHAR* ptr = tzInfo.StandardName;

    unicode_converter_c::to_utf8 (ptr, result);
    return result;
  }
  // -----------------------------------------------------------------
  std::string time_zone_c::dst_name ()
  {
    std::string result;
    TIME_ZONE_INFORMATION tzInfo;
    DWORD dstFlag = GetTimeZoneInformation (&tzInfo);
    WCHAR* ptr = tzInfo.DaylightName;

    unicode_converter_c::to_utf8 (ptr, result);
    return result;
  }
#else
  class TZInfo
  {
  public:
    TZInfo ()
    {
      tzset ();
    }
    
    int timeZone ()
    {
#if defined (__APPLE__)  || defined (__FreeBSD__) // no timezone global var
      std::time_t now = std::time (NULL);
      struct std::tm t;
      gmtime_r (&now, &t);
      std::time_t utc = std::mktime (&t);
      return now - utc;
#elif defined (__CYGWIN__)
      return -_timezone;
#else
      return -timezone;
#endif
    }
    const char* name (bool dst)
    {
      return tzname[dst ? 1 : 0];
    }
  };
  static TZInfo tzInfo;
  // -----------------------------------------------------------------
  int time_zone_c::utc_offset ()
  {
    return tzInfo.timeZone ();
  }
  // -----------------------------------------------------------------
  int time_zone_c::dst ()
  {
    std::time_t now = std::time (NULL);
    struct std::tm t;
    localtime_r (&now, &t);
    return t.tm_isdst == 1 ? 3600 : 0;
  }
  // -----------------------------------------------------------------
  bool time_zone_c::is_dst (const time_stamp_c& timestamp)
  {
    std::time_t time = timestamp.epoch_time ();
    struct std::tm* tms = std::localtime (&time);
    return tms->tm_isdst > 0;
  }
  // -----------------------------------------------------------------
  std::string time_zone_c::name ()
  {
    return std::string (tzInfo.name (dst () != 0));
  }
  // -----------------------------------------------------------------
  std::string time_zone_c::standard_name ()
  {
    return std::string (tzInfo.name (false));
  }
  // -----------------------------------------------------------------
  std::string time_zone_c::dst_name ()
  {
    return std::string (tzInfo.name (true));
  }
 
#endif
} 
