/**
 * @file   number_formatter.hpp
 * @author igor <igorgu@il.ibm.com>
 * @date   Wed Dec 23 22:48:07 2009
 * 
 * @brief  Definition of the number_formatter_c class.
 * 
 */

#ifndef __ABL_NUMBER_FORMATTER_HPP__
#define __ABL_NUMBER_FORMATTER_HPP__

#include <string>

#include "abl/abl_export.h"
#include "abl/types/basic_types.hpp"
#include "abl/types/types.hpp"

namespace abl 
{
  /**
   * The number_formatter_c class provides static methods
   * for formatting numeric values into strings.
   *
   * There are two kind of static member functions:
   *    * format* functions return a std::string containing
   *      the formatted value.
   *    * append* functions append the formatted value to
   *      an existing string.
   *
   * Internally, std::sprintf () is used to do the actual
   * formatting.
   */
  class ABL_API number_formatter_c

  {
  public:
    /**
     * Formats an integer value in decimal notation.
     */
    static std::string format (int value);
    /**
     * Formats an integer value in decimal notation,
     * right justified in a field having at least
     * the specified width.
     */
    static std::string format (int value, int width);
    /**
     * Formats an integer value in decimal notation, 
     * right justified and zero-padded in a field
     * having at least the specified width.
     */
    static std::string format0 (int value, int width);
    /**
     * Formats an int value in hexadecimal notation.
     * The value is treated as unsigned.
     */
    static std::string format_hex (int value);
    /**
     * Formats a int value in hexadecimal notation,
     * right justified and zero-padded in
     * a field having at least the specified width.
     * The value is treated as unsigned.
     */
    static std::string format_hex (int value, int width);
    /**
     * Formats an unsigned int value in decimal notation.
     */
    static std::string format (unsigned value);
    /**
     * Formats an unsigned long int in decimal notation,
     * right justified in a field having at least the
     * specified width.
     */
    static std::string format (unsigned value, int width);
    /**
     * Formats an unsigned int value in decimal notation, 
     * right justified and zero-padded in a field having at 
     * least the specified width.
     */
    static std::string format0 (unsigned int value, int width);
    /**
     * Formats an unsigned int value in hexadecimal notation.
     */
    static std::string format_hex (unsigned value);
    /**
     * Formats a int value in hexadecimal notation,
     * right justified and zero-padded in
     * a field having at least the specified width.
     */
    static std::string format_hex (unsigned value, int width);
    /**
     * Formats a long value in decimal notation.
     */
    static std::string format (long value);
    /**
     * Formats a long value in decimal notation,
     * right justified in a field having at least the 
     * specified width.
     */
    static std::string format (long value, int width);
    /**
     * Formats a long value in decimal notation, 
     * right justified and zero-padded in a field
     * having at least the specified width.
     */
    static std::string format0 (long value, int width);
    /**
     * Formats an unsigned long value in hexadecimal notation.
     * The value is treated as unsigned.
     */
    static std::string format_hex (long value);
    /**
     * Formats an unsigned long value in hexadecimal notation,
     * right justified and zero-padded in a field having at least the 
     * specified width.
     * The value is treated as unsigned.
     */
    static std::string format_hex (long value, int width);
    /*
     * Formats an unsigned long value in decimal notation.
     */
    static std::string format (unsigned long value);
    /**
     * Formats an unsigned long value in decimal notation,
     * right justified in a field having at least the specified 
     * width.
     */
    static std::string format (unsigned long value, int width);
    /**
     * Formats an unsigned long value in decimal notation, 
     * right justified and zero-padded
     * in a field having at least the specified width.
     */
    static std::string format0 (unsigned long value, int width);
    /**
     * Formats an unsigned long value in hexadecimal notation.
     */
    static std::string format_hex (unsigned long value);
    /**
     * Formats an unsigned long value in hexadecimal notation,
     * right justified and zero-padded in a field having at least the 
     * specified width.
     */
    static std::string format_hex (unsigned long value, int width);


#if defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
    /**
     * Formats a 64-bit integer value in decimal notation.
     */
    static std::string format (Int64 value);
    /**
     * Formats a 64-bit integer value in decimal notation,
     * right justified in a field having at least the specified width.
     */
    static std::string format (Int64 value, int width);
    /**
     * Formats a 64-bit integer value in decimal notation, 
     * right justified and zero-padded in a field having at least 
     * the specified width.
     */
    static std::string format0 (Int64 value, int width);
    /**
     * Formats a 64-bit integer value in hexadecimal notation.
     * The value is treated as unsigned.
     */
    static std::string format_hex (Int64 value);
    /**
     * Formats a 64-bit integer value in hexadecimal notation,
     * right justified and zero-padded in a field having at least 
     * the specified width.
     * The value is treated as unsigned.
     */
    static std::string format_hex (Int64 value, int width);
    /**
     * Formats an unsigned 64-bit integer value in decimal notation.
     */
    static std::string format (UInt64 value);
    /**
     * Formats an unsigned 64-bit integer value in decimal notation,
     * right justified in a field having at least the specified width.
     */
    static std::string format (UInt64 value, int width);
    /**
     * Formats an unsigned 64-bit integer value in decimal notation, 
     * right justified and zero-padded in a field having at least the 
     * specified width.
     */
    static std::string format0 (UInt64 value, int width);
    /**
     * Formats a 64-bit integer value in hexadecimal notation.
     */
    static std::string format_hex (UInt64 value);
    /**
     * Formats a 64-bit integer value in hexadecimal notation,
     * right justified and zero-padded in a field having at least 
     * the specified width.
     */
    static std::string format_hex (UInt64 value, int width);


#endif // defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
    /**
     * Formats a float value in decimal floating-point notation,
     * according to std::printf's %g format with a precision of 8 fractional digits.
     */
    static std::string format (float value);
    /**
     * Formats a double value in decimal floating-point notation,
     * according to std::printf's %g format with a precision of 16 fractional digits.
     */
    static std::string format (double value);
    /**
     * Formats a double value in decimal floating-point notation,
     * according to std::printf's %f format with the given precision.*/

    static std::string format (double value, int precision);
    /**
     * Formats a double value in decimal floating-point notation,
     * right justified in a field of the specified width,
     * with the number of fractional digits given in precision.
     */
    static std::string format (double value, int width, int precision);
    /**
     * Formats a pointer in an eight  (32-bit architectures) or
     * sixteen  (64-bit architectures) characters wide
     * field in hexadecimal notation.
     */
    static std::string format (const void* ptr);
    /**
     * Formats an integer value in decimal notation.
     */
    static void append (std::string& str, int value);
    /**
     * Formats an integer value in decimal notation,
     * right justified in a field having at least
     * the specified width.
     */
    static void append (std::string& str, int value, int width);
    /**
     * Formats an integer value in decimal notation, 
     * right justified and zero-padded in a field
     * having at least the specified width.
     */
    static void append0 (std::string& str, int value, int width);
    /**
     * Formats an int value in hexadecimal notation.
     * The value is treated as unsigned.
     */
    static void append_hex (std::string& str, int value);
    /**
     * Formats a int value in hexadecimal notation,
     * right justified and zero-padded in
     * a field having at least the specified width.
     * The value is treated as unsigned.
     */
    static void append_hex (std::string& str, int value, int width);
    /**
     * Formats an unsigned int value in decimal notation.
     */
    static void append (std::string& str, unsigned value);
    /**
     * Formats an unsigned long int in decimal notation,
     * right justified in a field having at least the
     * specified width.
     */
    static void append (std::string& str, unsigned value, int width);
    /**
     * Formats an unsigned int value in decimal notation, 
     * right justified and zero-padded in a field having at 
     * least the specified width.
     */
    static void append0 (std::string& str, unsigned int value, int width);
    /**
     * Formats an unsigned int value in hexadecimal notation.
     */
    static void append_hex (std::string& str, unsigned value);
    /**
     * Formats a int value in hexadecimal notation,
     * right justified and zero-padded in
     * a field having at least the specified width.
     */
    static void append_hex (std::string& str, unsigned value, int width);
    /**
     * Formats a long value in decimal notation.
     */
    static void append (std::string& str, long value);
    /**
     * Formats a long value in decimal notation,
     * right justified in a field having at least the 
     * specified width.
     */
    static void append (std::string& str, long value, int width);
    /**
     * Formats a long value in decimal notation, 
     * right justified and zero-padded in a field
     * having at least the specified width.
     */
    static void append0 (std::string& str, long value, int width);
    /**
     * Formats an unsigned long value in hexadecimal notation.
     * The value is treated as unsigned.
     */
    static void append_hex (std::string& str, long value);
    /**
     * Formats an unsigned long value in hexadecimal notation,
     * right justified and zero-padded in a field having at least the 
     * specified width.
     * The value is treated as unsigned.
     */
    static void append_hex (std::string& str, long value, int width);
    /**
     * Formats an unsigned long value in decimal notation.
     */
    static void append (std::string& str, unsigned long value);
    /**
     * Formats an unsigned long value in decimal notation,
     * right justified in a field having at least the specified 
     * width.
     */
    static void append (std::string& str, unsigned long value, int width);
    /**
     * Formats an unsigned long value in decimal notation, 
     * right justified and zero-padded
     * in a field having at least the specified width.
     */
    static void append0 (std::string& str, unsigned long value, int width);
    /**
     * Formats an unsigned long value in hexadecimal notation.
     */
    static void append_hex (std::string& str, unsigned long value);
    /**
     * Formats an unsigned long value in hexadecimal notation,
     * right justified and zero-padded in a field having at least the 
     * specified width.
     */
    static void append_hex (std::string& str, unsigned long value, int width);

#if defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
    /**
     * Formats a 64-bit integer value in decimal notation.
     */
    static void append (std::string& str, Int64 value);
    /**
     * Formats a 64-bit integer value in decimal notation,
     * right justified in a field having at least the specified width.
     */
    static void append (std::string& str, Int64 value, int width);
    /**
     * Formats a 64-bit integer value in decimal notation, 
     * right justified and zero-padded in a field having at least 
     * the specified width.
     */
    static void append0 (std::string& str, Int64 value, int width);
    /**
     * Formats a 64-bit integer value in hexadecimal notation.
     * The value is treated as unsigned.
     */
    static void append_hex (std::string& str, Int64 value);
    /**
     * Formats a 64-bit integer value in hexadecimal notation,
     * right justified and zero-padded in a field having at least 
     * the specified width.
     * The value is treated as unsigned.
     */
    static void append_hex (std::string& str, Int64 value, int width);
    /**
     * Formats an unsigned 64-bit integer value in decimal notation.
     */
    static void append (std::string& str, UInt64 value);
    /**
     * Formats an unsigned 64-bit integer value in decimal notation,
     * right justified in a field having at least the specified width.
     */
    static void append (std::string& str, UInt64 value, int width);
    /**
     * Formats an unsigned 64-bit integer value in decimal notation, 
     * right justified and zero-padded in a field having at least the 
     * specified width.
     */
    static void append0 (std::string& str, UInt64 value, int width);
    /*
     * Formats a 64-bit integer value in hexadecimal notation.
     */
    static void append_hex (std::string& str, UInt64 value);
    /**
     * Formats a 64-bit integer value in hexadecimal notation,
     * right justified and zero-padded in a field having at least 
     * the specified width.
     */
    static void append_hex (std::string& str, UInt64 value, int width);


#endif // defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
    /**
     * Formats a float value in decimal floating-point notation,
     * according to std::printf's %g format with a precision of 8 fractional digits.
     */
    static void append (std::string& str, float value);
    /**
     * Formats a double value in decimal floating-point notation,
     * according to std::printf's %g format with a precision of 16 fractional digits.
     */
    static void append (std::string& str, double value);
    /**
     * Formats a double value in decimal floating-point notation,
     * according to std::printf's %f format with the given precision.
     */
    static void append (std::string& str, double value, int precision);
    /**
     * Formats a double value in decimal floating-point notation,
     * right justified in a field of the specified width,
     * with the number of fractional digits given in precision.
     */
    static void append (std::string& str, double value, int width, int precision);
    /**
     * Formats a pointer in an eight  (32-bit architectures) or
     * sixteen  (64-bit architectures) characters wide
     * field in hexadecimal notation.
     */
    static void append (std::string& str, const void* ptr);

  };


  
} 


#endif 
