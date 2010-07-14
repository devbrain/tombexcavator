#ifndef __ABL_STRING_ENCODING_LATIN1_HPP__
#define __ABL_STRING_ENCODING_LATIN1_HPP__


#include "abl/abl_export.h"
#include "abl/string/text_encoding.hpp"


namespace abl
{

  /// ISO Latin-1 (8859-1) text encoding.
  class ABL_API latin1_encoding_c: public text_encoding_c
  {
  public:
    latin1_encoding_c ();
    ~latin1_encoding_c ();
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


} // namespace abl


#endif 
