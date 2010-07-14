#include "abl/string/utf8_string.hpp"
#include "abl/unicode/unicode.h"
#include "abl/string/text_iterator.hpp"
#include "abl/string/text_converter.hpp"
#include "abl/string/encoding/utf8.hpp"
#include <algorithm>
#include <assert.h>
#define poco_check_ptr assert

namespace abl 
{
  int utf8_s::icompare (const std::string&   str  , std::string::size_type pos, 
		       std::string::size_type n  , std::string::const_iterator it2, 
		       std::string::const_iterator end2)
  {
    static utf8_encoding_c utf8;
	
    std::string::size_type sz = str.size ();
    if  (pos > sz) pos = sz;
    if  (pos + n > sz) n = sz - pos;
    text_iterator_c uit1 (str.begin () + pos, str.begin () + pos + n, utf8); 
    text_iterator_c uend1 (str.begin () + pos + n);
    text_iterator_c uit2 (it2, end2, utf8);
    text_iterator_c uend2 (end2);
    while  (uit1 != uend1 && uit2 != uend2)
      {
        int c1 = unicode_to_lower  (*uit1);
        int c2 = unicode_to_lower  (*uit2);
        if  (c1 < c2)
	  return -1;
        else if  (c1 > c2)
	  return 1;
        ++uit1; ++uit2;
      }
    
    if  (uit1 == uend1)
      return uit2 == uend2 ? 0 : -1;
    else
      return 1;
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str1, const std::string& str2)
  {
    return icompare (str1, 0, str1.size (), str2.begin (), str2.end ());
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str1, std::string::size_type n1, 
		       const std::string& str2, std::string::size_type n2)
  {
    if  (n2 > str2.size ()) n2 = str2.size ();
    return icompare (str1, 0, n1, str2.begin (), str2.begin () + n2);
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str1, std::string::size_type n, const std::string& str2)
  {
    if  (n > str2.size ()) n = str2.size ();
    return icompare (str1, 0, n, str2.begin (), str2.begin () + n);
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str1, std::string::size_type pos, 
		       std::string::size_type n, const std::string& str2)
  {
    return icompare (str1, pos, n, str2.begin (), str2.end ());
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str1, std::string::size_type pos1, 
		       std::string::size_type n1, const std::string& str2, 
		       std::string::size_type pos2, std::string::size_type n2)
  {
    std::string::size_type sz2 = str2.size ();
    if  (pos2 > sz2) pos2 = sz2;
    if  (pos2 + n2 > sz2) n2 = sz2 - pos2;
    return icompare (str1, pos1, n1, str2.begin () + pos2, str2.begin () + pos2 + n2);
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str1, std::string::size_type pos1, 
		       std::string::size_type n, const std::string& str2, std::string::size_type pos2)
  {
    std::string::size_type sz2 = str2.size ();
    if  (pos2 > sz2) pos2 = sz2;
    if  (pos2 + n > sz2) n = sz2 - pos2;
    return icompare (str1, pos1, n, str2.begin () + pos2, str2.begin () + pos2 + n);
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str, std::string::size_type pos, 
		       std::string::size_type n, const std::string::value_type* ptr)
  {
    static utf8_encoding_c utf8;

    poco_check_ptr  (ptr);
    std::string::size_type sz = str.size ();
    if  (pos > sz) pos = sz;
    if  (pos + n > sz) n = sz - pos;
    text_iterator_c uit (str.begin () + pos, str.begin () + pos + n, utf8); 
    text_iterator_c uend (str.begin () + pos + n);
    while  (uit != uend && *ptr)
      {
        int c1 = unicode_to_lower  (*uit);
        int c2 = unicode_to_lower  (*ptr);
        if  (c1 < c2)
	  return -1;
        else if  (c1 > c2)
	  return 1;
        ++uit; ++ptr;
      }
    
    if  (uit == uend)
      return *ptr == 0 ? 0 : -1;
    else
      return 1;
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str, std::string::size_type pos, 
		       const std::string::value_type* ptr)
  {
    return icompare (str, pos, str.size () - pos, ptr);
  }
  // -------------------------------------------------------------------------------
  int utf8_s::icompare (const std::string& str, const std::string::value_type* ptr)
  {
    return icompare (str, 0, str.size (), ptr);
  }
  // -------------------------------------------------------------------------------
  std::string utf8_s::to_upper (const std::string& str)
  {
    static utf8_encoding_c utf8;
    std::string result;
    text_converter_c converter (utf8, utf8);
    converter.convert (str, result, unicode_to_upper);
    return result;
  }
  // -------------------------------------------------------------------------------
  std::string& utf8_s::to_upper_in_place (std::string& str)
  {
    static utf8_encoding_c utf8;
    std::string result;
    text_converter_c converter (utf8, utf8);
    converter.convert (str, result, unicode_to_upper);
    std::swap (str, result);
    return str;
  }
  // -------------------------------------------------------------------------------
  std::string utf8_s::to_lower (const std::string& str)
  {
    static utf8_encoding_c utf8;
    std::string result;
    text_converter_c converter (utf8, utf8);
    converter.convert (str, result, unicode_to_lower );
    return result;
  }
  // -------------------------------------------------------------------------------
  std::string& utf8_s::to_lower_in_place (std::string& str)
  {
    static utf8_encoding_c utf8;
    std::string result;
    text_converter_c converter (utf8, utf8);
    converter.convert (str, result, unicode_to_lower );
    std::swap (str, result);
    return str;
  }

} 
