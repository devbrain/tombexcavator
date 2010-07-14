/**
 * @file   environment.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Sun Dec 27 10:27:29 2009
 * 
 * @brief  Definition of the environment_c class.
 * 
 */

#ifndef __ABL_ENVIRONMENT_HPP__
#define __ABL_ENVIRONMENT_HPP__

#include "abl/abl_export.h"
#include "abl/types/types.hpp"

#include <string>

namespace abl 
{
  /**
   * This class provides access to environment variables
   * and some general system information.
   */
  class ABL_API environment_c

  {
  public:
    typedef int8_t node_id_t [6]; /**< Ethernet address. */
    /**
     * Returns the value of the environment variable
     * with the given name. Throws a NotFoundException
     * if the variable does not exist.
     */
    static std::string get (const std::string& name);
    /**
     * Returns the value of the environment variable
     * with the given name. If the environment variable
     * is undefined, returns defaultValue instead.
     */
    static std::string get (const std::string& name, const std::string& defaultValue);
    /**
     * Returns true iff an environment variable
     * with the given name is defined.
     */
    static bool has (const std::string& name);
    /**
     * Sets the environment variable with the given name
     * to the given value.
     */
    static void set (const std::string& name, const std::string& value);
    /**
     * Returns the operating system name.
     */
    static std::string os_name ();
    /**
     * Returns the operating system version.
     */
    static std::string os_version ();
    /**
     * Returns the operating system architecture.
     */
    static std::string os_architecture ();
    /**
     * Returns the node  (or host) name.
     */
    static std::string node_name ();
    /**
     * Returns the Ethernet address of the first Ethernet
     * adapter found on the system.
     *
     * Throws a SystemException if no Ethernet adapter is available.
     */
    static void node_id (node_id_t& id);
    /**
     * Returns the Ethernet address  (format "xx:xx:xx:xx:xx:xx")
     * of the first Ethernet adapter found on the system.
     *
     * Throws a SystemException if no Ethernet adapter is available.
     */
    static std::string node_id ();
    /**
     * Returns the number of processors installed in the system.
     *
     * If the number of processors cannot be determined, returns 1.
     */
    static unsigned processor_count ();

  };
} 
#endif 
