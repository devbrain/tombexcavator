#include "abl/string/utf9_string.hpp"
#include "abl/unicode/unicode.h"
#include "abl/string/text_iterator.hpp"
#include "abl/string/text_converter.hpp"
#include "abl/string/utf8_encoding.hpp"
#include <algorithm>


namespace abl 
{


  int utf8_s::icompare(const std::string& str, std::string::size_type pos, std::string::size_type n, std::string::const_iterator it2, std::string::const_iterator end2)
  {
    static UTF8Encoding utf8;
	
    std::string::size_type sz = str.size();
    if (pos > sz) pos = sz;
    if (pos + n > sz) n = sz - pos;
    TextIterator uit1(str.begin() + pos, str.begin() + pos + n, utf8); 
    TextIterator uend1(str.begin() + pos + n);
    TextIterator uit2(it2, end2, utf8);
    TextIterator uend2(end2);
    while (uit1 != uend1 && uit2 != uend2)
      {
        int c1 = Unicode::toLower(*uit1);
        int c2 = Unicode::toLower(*uit2);
        if (c1 < c2)
	  return -1;
        else if (c1 > c2)
	  return 1;
        ++uit1; ++uit2;
      }
    
    if (uit1 == uend1)
      return uit2 == uend2 ? 0 : -1;
    else
      return 1;
  }


  int utf8_s::icompare(const std::string& str1, const std::string& str2)
  {
    return icompare(str1, 0, str1.size(), str2.begin(), str2.end());
  }


  int utf8_s::icompare(const std::string& str1, std::string::size_type n1, const std::string& str2, std::string::size_type n2)
  {
    if (n2 > str2.size()) n2 = str2.size();
    return icompare(str1, 0, n1, str2.begin(), str2.begin() + n2);
  }


  int utf8_s::icompare(const std::string& str1, std::string::size_type n, const std::string& str2)
  {
    if (n > str2.size()) n = str2.size();
    return icompare(str1, 0, n, str2.begin(), str2.begin() + n);
  }


  int utf8_s::icompare(const std::string& str1, std::string::size_type pos, std::string::size_type n, const std::string& str2)
  {
    return icompare(str1, pos, n, str2.begin(), str2.end());
  }


  int utf8_s::icompare(const std::string& str1, std::string::size_type pos1, std::string::size_type n1, const std::string& str2, std::string::size_type pos2, std::string::size_type n2)
  {
    std::string::size_type sz2 = str2.size();
    if (pos2 > sz2) pos2 = sz2;
    if (pos2 + n2 > sz2) n2 = sz2 - pos2;
    return icompare(str1, pos1, n1, str2.begin() + pos2, str2.begin() + pos2 + n2);
  }


  int utf8_s::icompare(const std::string& str1, std::string::size_type pos1, std::string::size_type n, const std::string& str2, std::string::size_type pos2)
  {
    std::string::size_type sz2 = str2.size();
    if (pos2 > sz2) pos2 = sz2;
    if (pos2 + n > sz2) n = sz2 - pos2;
    return icompare(str1, pos1, n, str2.begin() + pos2, str2.begin() + pos2 + n);
  }


  int utf8_s::icompare(const std::string& str, std::string::size_type pos, std::string::size_type n, const std::string::value_type* ptr)
  {
    static UTF8Encoding utf8;

    poco_check_ptr (ptr);
    std::string::size_type sz = str.size();
    if (pos > sz) pos = sz;
    if (pos + n > sz) n = sz - pos;
    TextIterator uit(str.begin() + pos, str.begin() + pos + n, utf8); 
    TextIterator uend(str.begin() + pos + n);
    while (uit != uend && *ptr)
      {
        int c1 = Unicode::toLower(*uit);
        int c2 = Unicode::toLower(*ptr);
        if (c1 < c2)
	  return -1;
        else if (c1 > c2)
	  return 1;
        ++uit; ++ptr;
      }
    
    if (uit == uend)
      return *ptr == 0 ? 0 : -1;
    else
      return 1;
  }


  int utf8_s::icompare(const std::string& str, std::string::size_type pos, const std::string::value_type* ptr)
  {
    return icompare(str, pos, str.size() - pos, ptr);
  }


  int utf8_s::icompare(const std::string& str, const std::string::value_type* ptr)
  {
    return icompare(str, 0, str.size(), ptr);
  }


  std::string utf8_s::to_upper(const std::string& str)
  {
    static UTF8Encoding utf8;
    std::string result;
    TextConverter converter(utf8, utf8);
    converter.convert(str, result, Unicode::toUpper);
    return result;
  }


  std::string& utf8_s::to_upper_in_place(std::string& str)
  {
    static UTF8Encoding utf8;
    std::string result;
    TextConverter converter(utf8, utf8);
    converter.convert(str, result, Unicode::toUpper);
    std::swap(str, result);
    return str;
  }


  std::string utf8_s::to_lower(const std::string& str)
  {
    static UTF8Encoding utf8;
    std::string result;
    TextConverter converter(utf8, utf8);
    converter.convert(str, result, Unicode::toLower);
    return result;
  }


  std::string& utf8_s::to_lower_ln_place(std::string& str)
  {
    static UTF8Encoding utf8;
    std::string result;
    TextConverter converter(utf8, utf8);
    converter.convert(str, result, Unicode::toLower);
    std::swap(str, result);
    return str;
  }


} 
