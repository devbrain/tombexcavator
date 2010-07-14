#include "abl/number_parser.hpp"
#include "abl/exception.hpp"
#include <cstdio>
#include <cctype>


#if defined(_MSC_VER)
#define I64_FMT "I64"
#elif defined(__APPLE__) 
#define I64_FMT "q"
#else
#define I64_FMT "ll"
#endif


namespace abl 
{
  int number_parser_c::parse (const std::string& s)
  {
    int result;
    if  (try_parse (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid integer", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse (const std::string& s, int& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%d%c", &value, &temp) == 1;
  }
  // -----------------------------------------------------------------
  unsigned number_parser_c::parse_unsigned (const std::string& s)
  {
    unsigned result;
    if  (try_parse_unsigned (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid unsigned integer", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse_unsigned (const std::string& s, unsigned& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%u%c", &value, &temp) == 1;
  }
  // -----------------------------------------------------------------
  unsigned number_parser_c::parse_hex (const std::string& s)
  {
    unsigned result;
    if  (try_parse_hex (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid hexadecimal integer", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse_hex (const std::string& s, unsigned& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%x%c", &value, &temp) == 1;
  }
  // -----------------------------------------------------------------
#if defined (ABL_HAVE_INT64)
  Int64 number_parser_c::parse64 (const std::string& s)
  {
    Int64 result;
    if  (try_parse64 (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid integer", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse64 (const std::string& s, Int64& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%"I64_FMT"d%c", &value, &temp) == 1;
  }
  // -----------------------------------------------------------------
  UInt64 number_parser_c::parse_unsigned64 (const std::string& s)
  {
    UInt64 result;
    if  (try_parse_unsigned64 (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid unsigned integer", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse_unsigned64 (const std::string& s, UInt64& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%"I64_FMT"u%c", &value, &temp) == 1;
  }
  // -----------------------------------------------------------------
  UInt64 number_parser_c::parse_hex64 (const std::string& s)
  {
    UInt64 result;
    if  (try_parse_hex64 (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid hexadecimal integer", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse_hex64 (const std::string& s, UInt64& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%"I64_FMT"x%c", &value, &temp) == 1;
  }
  // -----------------------------------------------------------------
#endif // defined (ABL_HAVE_INT64)
  double number_parser_c::parse_float (const std::string& s)
  {
    double result;
    if  (try_parse_float (s, result))
      return result;
    else
      throw syntax_exception_c ("Not a valid floating-point number", s);
  }
  // -----------------------------------------------------------------
  bool number_parser_c::try_parse_float (const std::string& s, double& value)
  {
    char temp;
    return std::sscanf (s.c_str (), "%lf%c", &value, &temp) == 1;
  }
} 
