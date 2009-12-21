#ifndef __ABL_ENCODING_ASCII_HPP__
#define __ABL_ENCODING_ASCII_HPP__


#include "abl/abl_export.h"
#include "abl/string/text_encoding.hpp"


namespace abl 
{
  /// 7-bit ASCII text encoding.
  class ABL_API ascii_encoding_c: public text_encoding_c
  {
  public:
    ascii_encoding_c();
    ~ascii_encoding_c();
    const char* canonicalName() const;
    bool isA(const std::string& encodingName) const;
    const CharacterMap& characterMap() const;
    int convert(const unsigned char* bytes) const;
    int convert(int ch, unsigned char* bytes, int length) const;
    int queryConvert(const unsigned char* bytes, int length) const;
    int sequenceLength(const unsigned char* bytes, int length) const;
	
  private:
    static const char*        _names [];
    static const CharacterMap _charMap;
  };


} 


#endif 
