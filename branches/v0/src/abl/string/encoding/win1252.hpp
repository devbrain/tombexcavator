#ifndef __ABL_STRING_ENCODING_WIN1252_HPP__
#define __ABL_STRING_ENCODING_WIN1252_HPP__


#include "abl/abl_export.h"
#include "abl/string/text_encoding.hpp"


namespace abl {

  /// Windows Codepage 1252 text encoding.
  class ABL_API win1252_encoding_c: public text_encoding_c
  {
  public:
    win1252_encoding_c();
    ~win1252_encoding_c();
    const char* canonicalName() const;
    bool isA(const std::string& encodingName) const;
    const CharacterMap& characterMap() const;
    int convert(const unsigned char* bytes) const;
    int convert(int ch, unsigned char* bytes, int length) const;
    int queryConvert(const unsigned char* bytes, int length) const;
    int sequenceLength(const unsigned char* bytes, int length) const;
	
  private:
    static const char* _names[];
    static const CharacterMap _charMap;
  };


} 


#endif 
