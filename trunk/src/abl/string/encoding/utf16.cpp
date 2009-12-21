#include "abl/string/encoding/utf16.hpp"
#include "abl/byte_order.hpp"
#include "abl/string/string.hpp"


namespace abl 
{


  const char* utf16_encoding_C::_names[] =
    {
      "UTF-16",
      "UTF16",
      NULL
    };


  const text_encoding_c::CharacterMap utf16_encoding_C::_charMap = 
    {
      /* 00 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 10 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 20 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 30 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 40 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 50 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 60 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 70 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 80 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* 90 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* a0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* b0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* c0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* d0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* e0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
      /* f0 */	-2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 
    };


  utf16_encoding_C::utf16_encoding_C(ByteOrderType byteOrder)
  {
    setByteOrder (byteOrder);
  }

	
  utf16_encoding_C::utf16_encoding_C(int byteOrderMark)
  {
    setByteOrder (byteOrderMark);
  }

	
  utf16_encoding_C::~utf16_encoding_C()
  {
  }


  utf16_encoding_C::ByteOrderType utf16_encoding_C::getByteOrder () const
  {
#if defined(TE_ARCH_BIG_ENDIAN)
    return _flipBytes ? LITTLE_ENDIAN_BYTE_ORDER : BIG_ENDIAN_BYTE_ORDER;
#else
    return _flipBytes ? BIG_ENDIAN_BYTE_ORDER : LITTLE_ENDIAN_BYTE_ORDER;
#endif
  }

	
  void utf16_encoding_C::setByteOrder (ByteOrderType byteOrder)
  {
#if defined(TE_ARCH_BIG_ENDIAN)
    _flipBytes = byteOrder == LITTLE_ENDIAN_BYTE_ORDER;
#else
    _flipBytes = byteOrder == BIG_ENDIAN_BYTE_ORDER;;
#endif
  }

	
  void utf16_encoding_C::setByteOrder (int byteOrderMark)
  {
    _flipBytes = byteOrderMark != 0xFEFF;
  }


  const char* utf16_encoding_C::canonicalName() const
  {
    return _names[0];
  }


  bool utf16_encoding_C::isA(const std::string& encodingName) const
  {
    for (const char** name = _names; *name; ++name)
      {
	if (abl::icompare(encodingName, *name) == 0)
	  return true;
      }
    return false;
  }


  const text_encoding_c::CharacterMap& utf16_encoding_C::characterMap() const
  {
    return _charMap;
  }


  int utf16_encoding_C::convert(const unsigned char* bytes) const
  {
    te_uint16_t uc;
    unsigned char* p = (unsigned char*) &uc;
    *p++ = *bytes++;
    *p++ = *bytes++;

    if (_flipBytes)
      {
	byte_order_c::flipBytes(uc);
      }

    if (uc >= 0xd800 && uc < 0xdc00)
      {
	te_uint16_t uc2;
	p = (unsigned char*) &uc2;
	*p++ = *bytes++;
	*p++ = *bytes++;

	if (_flipBytes)
	  {
	    byte_order_c::flipBytes(uc2);
	  }
	if (uc2 >= 0xdc00 && uc2 < 0xe000)
	  {
	    return ((uc & 0x3ff) << 10) + (uc2 & 0x3ff) + 0x10000;
	  }
	else
	  {
	    return -1;
	  }
      }
    else
      {
	return uc;
      }
  }


  int utf16_encoding_C::convert(int ch, unsigned char* bytes, int length) const
  {
    if (ch <= 0xFFFF)
      {
	if (bytes && length >= 2)
	  {
	    te_uint16_t ch1 = _flipBytes ? byte_order_c::flipBytes((te_uint16_t) ch) : (te_uint16_t) ch;
	    unsigned char* p = (unsigned char*) &ch1;
	    *bytes++ = *p++;
	    *bytes++ = *p++;
	  }
	return 2;
      }
    else
      {
	if (bytes && length >= 4)
	  {
	    int ch1 = ch - 0x10000;
	    te_uint16_t w1 = 0xD800 + ((ch1 >> 10) & 0x3FF);
	    te_uint16_t w2 = 0xDC00 + (ch1 & 0x3FF);
	    if (_flipBytes)
	      {
		w1 = byte_order_c::flipBytes(w1);
		w2 = byte_order_c::flipBytes(w2);
	      }
	    unsigned char* p = (unsigned char*) &w1;
	    *bytes++ = *p++;
	    *bytes++ = *p++;
	    p = (unsigned char*) &w2;
	    *bytes++ = *p++;
	    *bytes++ = *p++;
	  }
	return 4;
      }
  }


  int utf16_encoding_C::queryConvert(const unsigned char* bytes, int length) const
  {
    int ret = -2;

    if (length >= 2)
      {
	te_uint16_t uc;
	unsigned char* p = (unsigned char*) &uc;
	*p++ = *bytes++;
	*p++ = *bytes++;
	if (_flipBytes) 
	  byte_order_c::flipBytes(ret);
	if (uc >= 0xd800 && uc < 0xdc00)
	  {
	    if (length >= 4)
	      {
		te_uint16_t uc2;
		p = (unsigned char*) &uc2;
		*p++ = *bytes++;
		*p++ = *bytes++;
		if (_flipBytes) 
		  byte_order_c::flipBytes(ret);
		if (uc2 >= 0xdc00 && uc < 0xe000)
		  {
		    ret = ((uc & 0x3ff) << 10) + (uc2 & 0x3ff) + 0x10000;
		  }
		else
		  {
		    ret = -1;	// Malformed sequence
		  }
	      }
	    else
	      {
		ret = -4;	// surrogate pair, four bytes needed
	      }
	  }
	else
	  {
	    ret = uc;
	  }
      }

    return ret;
  }


  int utf16_encoding_C::sequenceLength(const unsigned char* bytes, int length) const
  {
    int ret = -2;

    if (_flipBytes)
      {
	if (length >= 1)
	  {
	    unsigned char c = *bytes;
	    if (c >= 0xd8 && c < 0xdc)
	      ret = 4;
	    else
	      ret = 2;
	  }
      }
    else
      {
	if (length >= 2)
	  {
	    te_uint16_t uc;
	    unsigned char* p = (unsigned char*) &uc;
	    *p++ = *bytes++;
	    *p++ = *bytes++;
	    if (uc >= 0xd800 && uc < 0xdc00)
	      ret = 4;
	    else
	      ret = 2;
	  }
      }
    return ret;
  }


} // namespace abl
