/**
 * @file   time_zone.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 16:12:14 2009
 * 
 * @brief  Definition of the time_zone_c class.
 */

#ifndef __ABL_DATE_TIME_TIME_ZONE_HPP__
#define __ABL_DATE_TIME_TIME_ZONE_HPP__


#include "abl/abl_export.h"
#include "abl/date_time/time_stamp.hpp"
#include <string>

namespace abl 
{
  /**
   * This class provides information about the current timezone.
   */
  class ABL_API time_zone_c
  {
  public:
    /**
     * Returns the offset of local time to UTC, in seconds.
     *     local time = UTC + utcOffset() + dst().
     */
    static int utc_offset ();
    /**
     * Returns the daylight saving time offset in seconds if
     * daylight saving time is in use.
     *     local time = UTC + utcOffset() + dst().
     */
    static int dst ();
    /**
     * Returns true if daylight saving time is in effect
     * for the given time. Depending on the operating system
     * platform this might only work reliably for certain
     * date ranges, as the C library's localtime() function
     * is used.
     */
    static bool is_dst(const time_stamp_c& timestamp);
    /**
     * Returns the time zone differential for the current timezone.
     * The timezone differential is computed as utcOffset() + dst()
     * and is expressed in seconds.
     */
    static int tzd ();
    /**
     * Returns the timezone name currently in effect.
     */
    static std::string name ();
    /**
     * Returns the timezone name if not daylight saving time is in effect.
     */
    static std::string standard_name ();
    /**
     * Returns the timezone name if daylight saving time is in effect.
     */
    static std::string dst_name ();
		
  };


} 


#endif 
