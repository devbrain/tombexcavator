/**
 * @file   number_parser.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Thu Dec 24 19:51:25 2009
 * 
 * @brief Definition of the number_parser_c class. 
 * 
 */

#ifndef __ABL_NUMBER_PARSER_HPP__
#define __ABL_NUMBER_PARSER_HPP__


#include "abl/abl_export.h"
#include "abl/types/types.hpp"
#include <string>

namespace abl 
{
  /**
   * The number_parser_c class provides static methods
   * for parsing numbers out of strings.
   */
  class ABL_API number_parser_c
  {
  public:
    /**
     * Parses an integer value in decimal notation from the given string.
     * Throws a SyntaxException if the string does not hold a number in decimal notation.
     */
    static int parse (const std::string& s);
    /**
     * Parses an integer value in decimal notation from the given string.
     * Returns true if a valid integer has been found, false otherwise. 
     */
    static bool try_parse (const std::string& s, int& value);
    /**
     * Parses an unsigned integer value in decimal notation from the given string.
     * Throws a SyntaxException if the string does not hold a number in decimal notation.
     */
    static unsigned parse_unsigned (const std::string& s);
    /**
     * Parses an unsigned integer value in decimal notation from the given string.
     * Returns true if a valid integer has been found, false otherwise. 
     */
    static bool try_parse_unsigned (const std::string& s, unsigned& value);
    /**
     * Parses an integer value in hexadecimal notation from the given string.
     * Throws a SyntaxException if the string does not hold a number in
     * hexadecimal notation.
     */
    static unsigned parse_hex (const std::string& s);
    /**
     * Parses an unsigned integer value in hexadecimal notation from the given string.
     * Returns true if a valid integer has been found, false otherwise. 
     */
    static bool try_parse_hex (const std::string& s, unsigned& value);


#if defined (ABL_HAVE_INT64)
    /**
     * Parses a 64-bit integer value in decimal notation from the given string.
     * Throws a SyntaxException if the string does not hold a number in decimal notation.
     */
    static int64_t parse64 (const std::string& s);
    /**
     * Parses a 64-bit integer value in decimal notation from the given string.
     * Returns true if a valid integer has been found, false otherwise. 
     */
    static bool try_parse64 (const std::string& s, int64_t& value);
    /**
     * Parses an unsigned 64-bit integer value in decimal notation from the given string.
     * Throws a SyntaxException if the string does not hold a number in decimal notation.
     */
    static uint64_t parse_unsigned64 (const std::string& s);
    /**
     * Parses an unsigned 64-bit integer value in decimal notation from the given string.
     * Returns true if a valid integer has been found, false otherwise. 
     */
    static bool try_parse_unsigned64 (const std::string& s, uint64_t& value);
    /**
     * Parses a 64 bit-integer value in hexadecimal notation from the given string.
     * Throws a SyntaxException if the string does not hold a number in hexadecimal notation.
     */
    static uint64_t parse_hex64 (const std::string& s);
    /**
     * Parses an unsigned 64-bit integer value in hexadecimal notation from the given string.
     * Returns true if a valid integer has been found, false otherwise. 
     */
    static bool try_parse_hex64 (const std::string& s, uint64_t& value);

#endif // defined (ABL_HAVE_INT64)
    /**
     * Parses a double value in decimal floating point notation
     * from the given string. 
     * Throws a SyntaxException if the string does not hold a floating-point 
     * number in decimal notation.
     */
    static double parse_float (const std::string& s);
    /**
     * Parses a double value in decimal floating point notation
     * from the given string. 
     * Returns true if a valid floating point number has been found, 
     * false otherwise. 		
     */
    static bool try_parse_float (const std::string& s, double& value);

  };


} // namespace Poco


#endif 
