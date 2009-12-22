#include "abl/string/text_converter.hpp"
#include "abl/string/text_iterator.hpp"
#include "abl/string/text_encoding.hpp"

#include <assert.h>
#define poco_assert assert
#define poco_check_ptr assert

namespace 
{
  int null_transform(int ch)
  {
    return ch;
  }
}


namespace abl 
{


  text_converter_c::text_converter_c (const text_encoding_c& inencoding, 
				      const text_encoding_c& outencoding, 
				      int default_char)
    : m_in_encoding(inencoding),
      m_out_encoding(outencoding),
      m_default_char(default_char)
  {
  }
  // -------------------------------------------------------------------------------
  text_converter_c::~text_converter_c ()
  {
  }
  // -------------------------------------------------------------------------------
  int text_converter_c::convert (const std::string& source, std::string& destination, transform_t trans)
  {
    int errors = 0;
    text_iterator_c it  (source, m_in_encoding);
    text_iterator_c end (source);
    unsigned char buffer [text_encoding_c::MAX_SEQUENCE_LENGTH];

    while (it != end)
      {
	int c = *it;
	if (c == -1) 
	  { 
	    ++errors; 
	    c = m_default_char; 
	  }
	c = trans(c);
	int n = m_out_encoding.convert (c, buffer, sizeof(buffer));
	if (n == 0) n = m_out_encoding.convert (m_default_char, buffer, sizeof(buffer));
	poco_assert (n <= sizeof(buffer));
	destination.append ((const char*) buffer, n);
	++it;
      }
    return errors;
  }
  // -------------------------------------------------------------------------------
  int text_converter_c::convert (const void* source, int length, std::string& destination, transform_t trans)
  {
    poco_check_ptr (source);

    int errors = 0;
    const unsigned char* it  = (const unsigned char*) source;
    const unsigned char* end = (const unsigned char*) source + length;
    unsigned char buffer [text_encoding_c::MAX_SEQUENCE_LENGTH];
	
    while (it < end)
      {
	int n = m_in_encoding.queryConvert (it, 1);
	int uc;
	int read = 1;

	while (-1 > n && (end - it) >= -n)
	  {
	    read = -n;
	    n = m_in_encoding.queryConvert (it, read);
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
	    uc = m_default_char;
	    ++errors;
	  }
	else
	  {
	    uc = n;
	  }

	uc = trans (uc);
	n = m_out_encoding.convert (uc, buffer, sizeof(buffer));
	if (n == 0) n = m_out_encoding.convert (m_default_char, buffer, sizeof(buffer));
	poco_assert (n <= sizeof(buffer));
	destination.append ((const char*) buffer, n);
      }
    return errors;
  }
  // -------------------------------------------------------------------------------
  int text_converter_c::convert (const std::string& source, std::string& destination)
  {
    return convert (source, destination, null_transform);
  }
  // -------------------------------------------------------------------------------
  int text_converter_c::convert (const void* source, int length, std::string& destination)
  {
    return convert (source, length, destination, null_transform);
  }
} // namespace Poco
