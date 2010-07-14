#include "abl/string/unicode_converter.hpp"
#include "abl/string/text_converter.hpp"
#include "abl/string/text_iterator.hpp"
#include "abl/string/encoding/utf8.hpp"
#include "abl/string/encoding/utf16.hpp"

#include <cstring>
#include <wchar.h>
#include <assert.h>
#define poco_assert assert
#define poco_check_ptr assert

namespace abl 
{
  void unicode_converter_c::to_utf16 (const std::string& utf8string, std::wstring& utf16string)
  {
    utf16string.clear ();
    utf8_encoding_c utf8encoding;
    text_iterator_c it  (utf8string, utf8encoding);
    text_iterator_c end (utf8string);
    while (it != end) 
      {
	int cc = *it++;
	if (cc <= 0xffff)
	  {
	    utf16string += (wchar_t) cc;
	  }
	else
	  {
	    cc -= 0x10000;
	    utf16string += (wchar_t) ((cc >> 10) & 0x3ff) | 0xd800;
	    utf16string += (wchar_t) (cc & 0x3ff) | 0xdc00;
	  }
      }
  }
  // -------------------------------------------------------------------------------------------------------
  void unicode_converter_c::to_utf16 (const char* utf8string, int length, std::wstring& utf16string)
  {
    poco_check_ptr (utf8string);

    utf16string.clear ();

    utf8_encoding_c  utf8encoding;
    utf16_encoding_c utf16encoding;
    const unsigned char* it  = (const unsigned char*) utf8string;
    const unsigned char* end = (const unsigned char*) utf8string + length;
	
    while (it < end)
      {
	int n = utf8encoding.queryConvert(it, 1);
	int uc;
	int read = 1;

	while (-1 > n && (end - it) >= -n)
	  {
	    read = -n;
	    n = utf8encoding.queryConvert(it, read);
	  }
		
	if (-1 > n)
	  {
	    it = end;
	  }
	else
	  {
	    it += read;
	  }

	if (-1 >= n)
	  {
	    uc = 0xfffd;	// Replacement Character (instead of '?')
	  }
	else
	  {
	    uc = n;
	  }

	if (uc > 0xffff)
	  {
	    uc -= 0x10000;
	    utf16string += (wchar_t) ((uc >> 10) & 0x3ff) | 0xd800 ;
	    utf16string += (wchar_t) (uc & 0x3ff) | 0xdc00 ;
	  }
	else
	  {
	    utf16string += (wchar_t) uc;
	  }
      }
  }
  // -------------------------------------------------------------------------------------------------------
  void unicode_converter_c::to_utf16(const char* utf8string, std::wstring& utf16string)
  {
    to_utf16 (utf8string, (int) std::strlen(utf8string), utf16string);
  }
  // -------------------------------------------------------------------------------------------------------
  void unicode_converter_c::to_utf8(const std::wstring& utf16string, std::string& utf8string)
  {
    utf8string.clear ();
    utf8_encoding_c  utf8encoding;
    utf16_encoding_c utf16encoding;
    text_converter_c converter (utf16encoding, utf8encoding);
    converter.convert (utf16string.data (), (int) utf16string.length ()*sizeof(wchar_t), utf8string);
  }
  // -------------------------------------------------------------------------------------------------------
  void unicode_converter_c::to_utf8(const wchar_t* utf16string, int length, std::string& utf8string)
  {
    utf8string.clear ();
    utf8_encoding_c  utf8encoding;
    utf16_encoding_c utf16encoding;
    text_converter_c converter (utf16encoding, utf8encoding);
    converter.convert (utf16string, (int) length*sizeof(wchar_t), utf8string);
  }
  // -------------------------------------------------------------------------------------------------------
  void unicode_converter_c::to_utf8 (const wchar_t* utf16string, std::string& utf8string)
  {
    to_utf8 (utf16string, (int) wcslen (utf16string), utf8string);
  }

} 



