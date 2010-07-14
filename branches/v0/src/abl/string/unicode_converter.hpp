/**
 * @file   unicode_converter.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 14:51:57 2009
 * 
 * @brief  Definition of the unicode_converter_c class.
 */


#ifndef __ABL_STRING_UNICODE_CONVERTER_HPP__
#define __ABL_STRING_UNICODE_CONVERTER_HPP__


#include "abl/abl_export.h"
#include <string>

namespace abl 
{
  /**
   * A convenience class that converts strings from
   * UTF-8 encoded std::strings to UTF-16 encoded std::wstrings 
   * and vice-versa.
   *
   * This class is mainly used for working with the Unicode Windows APIs
   * and probably won't be of much use anywhere else.
   */

  class ABL_API unicode_converter_c
  {
  public:
    static void to_utf16 (const std::string& utf8string, std::wstring& utf16string);
    static void to_utf16 (const char* utf8string, int length, std::wstring& utf16string);	
    static void to_utf16 (const char* utf8string, std::wstring& utf16string);	

    static void to_utf8 (const std::wstring& utf16string, std::string& utf8string);
    static void to_utf8 (const wchar_t* utf16string, int length, std::string& utf8string);
    static void to_utf8 (const wchar_t* utf16string, std::string& utf8string);
  };


} 


#endif 
