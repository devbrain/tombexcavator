#ifndef __ABL_STRING_ENCODING_UTF16_HPP__
#define __ABL_STRING_ENCODING_UTF16_HPP__


#include "abl/abl_export.h"
#include "abl/string/text_encoding.hpp"


namespace abl 
{
  
  /// UTF-16 text encoding, as defined in RFC 2781.
///
/// When converting from UTF-16 to Unicode, surrogates are
/// reported as they are - in other words, surrogate pairs
/// are not combined into one Unicode character. 
/// When converting from Unicode to UTF-16, however, characters
/// outside the 16-bit range are converted into a low and
/// high surrogate.
  class ABL_API utf16_encoding_C: public text_encoding_c
  {
  public:
    enum ByteOrderType
      {
	BIG_ENDIAN_BYTE_ORDER,
	LITTLE_ENDIAN_BYTE_ORDER,
	NATIVE_BYTE_ORDER
      };
	
    utf16_encoding_C(ByteOrderType byteOrder = NATIVE_BYTE_ORDER);
    /// Creates and initializes the encoding for the given byte order.
		
    utf16_encoding_C(int byteOrderMark);
    /// Creates and initializes the encoding for the byte-order
    /// indicated by the given byte-order mark, which is the Unicode
    /// character 0xFEFF.
		
    ~utf16_encoding_C();
	
    ByteOrderType getByteOrder() const;
    /// Returns the byte-order currently in use.
		
    void setByteOrder(ByteOrderType byteOrder);
    /// Sets the byte order.
		
    void setByteOrder(int byteOrderMark);
    /// Sets the byte order according to the given
    /// byte order mark, which is the Unicode
    /// character 0xFEFF.
	
    const char* canonicalName() const;
    bool isA(const std::string& encodingName) const;
    const CharacterMap& characterMap() const;
    int convert(const unsigned char* bytes) const;
    int convert(int ch, unsigned char* bytes, int length) const;
    int queryConvert(const unsigned char* bytes, int length) const;
    int sequenceLength(const unsigned char* bytes, int length) const;
	
  private:
    bool _flipBytes;
    static const char* _names[];
    static const CharacterMap _charMap;
  };


} // namespace Poco


#endif 

