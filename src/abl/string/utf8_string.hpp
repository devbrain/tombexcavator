#ifndef __ABL_STRING_UTF8_STRING_HPP__
#define __ABL_STRING_UTF8_STRING_HPP__

#include <string>
#include "abl/abl_export.h" 

namespace abl 
{

  struct ABL_API utf8_s
  {
    static int icompare(const std::string& str  , std::string::size_type pos, 
			std::string::size_type n, std::string::const_iterator it2, std::string::const_iterator end2);
    static int icompare(const std::string& str1, const std::string& str2);
    static int icompare(const std::string& str1, std::string::size_type n1, const std::string& str2, 
			std::string::size_type n2);
    static int icompare(const std::string& str1, std::string::size_type n, const std::string& str2);
    static int icompare(const std::string& str1, std::string::size_type pos, std::string::size_type n, 
			const std::string& str2);
    static int icompare(const std::string& str1, std::string::size_type pos1, std::string::size_type n1, 
			const std::string& str2, std::string::size_type pos2, std::string::size_type n2);
    static int icompare(const std::string& str1, std::string::size_type pos1, std::string::size_type n, 
			const std::string& str2, std::string::size_type pos2);
    static int icompare(const std::string& str, std::string::size_type pos, std::string::size_type n, 
			const std::string::value_type* ptr);
    static int icompare(const std::string& str, std::string::size_type pos, const std::string::value_type* ptr);
    static int icompare(const std::string& str, const std::string::value_type* ptr);
    
    static std::string to_upper           (const std::string& str);
    static std::string& to_upper_in_place (std::string& str);
    static std::string to_lower           (const std::string& str);
    static std::string& to_lower_in_place (std::string& str);
  };

} 


#endif 

