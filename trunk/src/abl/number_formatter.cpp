#include "abl/number_formatter.hpp"
#include <cstdio>
#include <cctype>
#include <assert.h>

#define poco_assert assert

#if defined(_MSC_VER)
#define I64_FMT "I64"
#elif defined(__APPLE__) 
#define I64_FMT "q"
#else
#define I64_FMT "ll"
#endif


namespace abl 
{
  void number_formatter_c::append (std::string& str, int value)
  {
    char buffer[64];
    std::sprintf (buffer, "%d", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, int value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%*d", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append0 (std::string& str, int value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*d", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, int value)
  {
    char buffer[64];
    std::sprintf (buffer, "%X", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, int value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*X", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, unsigned value)
  {
    char buffer[64];
    std::sprintf (buffer, "%u", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, unsigned value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%*u", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append0 (std::string& str, unsigned int value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*u", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, unsigned value)
  {
    char buffer[64];
    std::sprintf (buffer, "%X", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, unsigned value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*X", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, long value)
  {
    char buffer[64];
    std::sprintf (buffer, "%ld", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, long value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%*ld", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append0 (std::string& str, long value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*ld", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, long value)
  {
    char buffer[64];
    std::sprintf (buffer, "%lX", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, long value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*lX", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, unsigned long value)
  {
    char buffer[64];
    std::sprintf (buffer, "%lu", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, unsigned long value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%*lu", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append0 (std::string& str, unsigned long value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*lu", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, unsigned long value)
  {
    char buffer[64];
    std::sprintf (buffer, "%lX", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, unsigned long value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*lX", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
#if defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
  void number_formatter_c::append (std::string& str, Int64 value)
  {
    char buffer[64];
    std::sprintf (buffer, "%"I64_FMT"d", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, Int64 value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%*"I64_FMT"d", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append0 (std::string& str, Int64 value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*"I64_FMT"d", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, Int64 value)
  {
    char buffer[64];
    std::sprintf (buffer, "%"I64_FMT"X", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, Int64 value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*"I64_FMT"X", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, UInt64 value)
  {
    char buffer[64];
    std::sprintf (buffer, "%"I64_FMT"u", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, UInt64 value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%*"I64_FMT"u", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append0 (std::string& str, UInt64 value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*"I64_FMT"u", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, UInt64 value)
  {
    char buffer[64];
    std::sprintf (buffer, "%"I64_FMT"X", value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append_hex (std::string& str, UInt64 value, int width)
  {
    poco_assert  (width > 0 && width < 64);

    char buffer[64];
    std::sprintf (buffer, "%0*"I64_FMT"X", width, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
#endif // defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
  void number_formatter_c::append (std::string& str, float value)
  {
    char buffer[64];
    std::sprintf (buffer, "%.*g", 8,  (double) value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, double value)
  {
    char buffer[64];
    std::sprintf (buffer, "%.*g", 16, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, double value, int precision)
  {
    poco_assert  (precision >= 0 && precision < 32);

    char buffer[64];
    std::sprintf (buffer, "%.*f", precision, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, double value, int width, int precision)
  {
    poco_assert  (width > 0 && width < 64 && precision >= 0 && precision < width);

    char buffer[64];
    std::sprintf (buffer, "%*.*f", width, precision, value);
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  void number_formatter_c::append (std::string& str, const void* ptr)
  {
    char buffer[24];
#if defined (ABL_PTR_IS_64_BIT)
    std::sprintf (buffer, "%016"I64_FMT"X",  (UIntPtr) ptr);
#else
    std::sprintf (buffer, "%08lX",  (UIntPtr) ptr);
#endif
    str.append (buffer);
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (int value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (int value, int width)
  {
    std::string result;
    append (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format0 (int value, int width)
  {
    std::string result;
    append0 (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (int value)
  {
    std::string result;
    append_hex (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (int value, int width)
  {
    std::string result;
    append_hex (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (unsigned value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (unsigned value, int width)
  {
    std::string result;
    append (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format0 (unsigned int value, int width)
  {
    std::string result;
    append0 (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (unsigned value)
  {
    std::string result;
    append_hex (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (unsigned value, int width)
  {
    std::string result;
    append_hex (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (long value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (long value, int width)
  {
    std::string result;
    append (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format0 (long value, int width)
  {
    std::string result;
    append0 (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (long value)
  {
    std::string result;
    append_hex (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (long value, int width)
  {
    std::string result;
    append_hex (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (unsigned long value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (unsigned long value, int width)
  {
    std::string result;
    append (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format0 (unsigned long value, int width)
  {
    std::string result;
    append0 (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (unsigned long value)
  {
    std::string result;
    append_hex (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (unsigned long value, int width)
  {
    std::string result;
    append_hex (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
#if defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
  std::string number_formatter_c::format (Int64 value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (Int64 value, int width)
  {
    std::string result;
    append (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format0 (Int64 value, int width)
  {
    std::string result;
    append0 (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (Int64 value)
  {
    std::string result;
    append_hex (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (Int64 value, int width)
  {
    std::string result;
    append_hex (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (UInt64 value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (UInt64 value, int width)
  {
    std::string result;
    append (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format0 (UInt64 value, int width)
  {
    std::string result;
    append0 (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (UInt64 value)
  {
    std::string result;
    append_hex (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format_hex (UInt64 value, int width)
  {
    std::string result;
    append_hex (result, value, width);
    return result;
  }
  // ---------------------------------------------------------------
#endif // defined (ABL_HAVE_INT64) && !defined (ABL_LONG_IS_64_BIT)
  std::string number_formatter_c::format (float value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (double value)
  {
    std::string result;
    append (result, value);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (double value, int precision)
  {
    std::string result;
    append (result, value, precision);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (double value, int width, int precision)
  {
    std::string result;
    append (result, value, width, precision);
    return result;
  }
  // ---------------------------------------------------------------
  std::string number_formatter_c::format (const void* ptr)
  {
    std::string result;
    append (result, ptr);
    return result;
  }
} 
