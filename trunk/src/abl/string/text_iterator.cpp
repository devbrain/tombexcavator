#include "abl/string/text_iterator.hpp"
#include "abl/string/text_encoding.hpp"
#include <algorithm> 
#include <assert.h>

#define poco_check_ptr assert
#define poco_assert assert

namespace abl 
{
  text_iterator_c::text_iterator_c():
    _pEncoding(0)
  {
  }


  text_iterator_c::text_iterator_c(const std::string& str, const text_encoding_c& encoding):
    _pEncoding(&encoding),
    _it(str.begin()),
    _end(str.end())
  {
  }

  text_iterator_c::text_iterator_c(const std::string::const_iterator& begin, const std::string::const_iterator& end, const text_encoding_c& encoding):
    _pEncoding(&encoding),
    _it(begin),
    _end(end)
  {
  }


  text_iterator_c::text_iterator_c(const std::string& str):
    _pEncoding(0),
    _it(str.end()),
    _end(str.end())
  {
  }


  text_iterator_c::text_iterator_c(const std::string::const_iterator& end):
    _pEncoding(0),
    _it(end),
    _end(end)
  {
  }


  text_iterator_c::~text_iterator_c()
  {
  }


  text_iterator_c::text_iterator_c(const text_iterator_c& it):
    _pEncoding(it._pEncoding),
    _it(it._it),
    _end(it._end)
  {
  }


  text_iterator_c& text_iterator_c::operator = (const text_iterator_c& it)
  {
    if (&it != this)
      {
	_pEncoding = it._pEncoding;
	_it        = it._it;
	_end       = it._end;
      }
    return *this;
  }


  void text_iterator_c::swap(text_iterator_c& it)
  {
    std::swap(_pEncoding, it._pEncoding);
    std::swap(_it, it._it);
    std::swap(_end, it._end);
  }


  int text_iterator_c::operator * () const
  {
    poco_check_ptr (_pEncoding);
    poco_assert (_it != _end);
    std::string::const_iterator it = _it;
	
    unsigned char buffer[text_encoding_c::MAX_SEQUENCE_LENGTH];
    unsigned char* p = buffer;

    if (it != _end)
      *p++ = *it++;
    else
      *p++ = 0;

    int read = 1;
    int n = _pEncoding->queryConvert(buffer, 1);

    while (-1 > n && (_end - it) >= -n - read)
      {
	while (read < -n && it != _end)
	  { 
	    *p++ = *it++; 
	    read++; 
	  }
	n = _pEncoding->queryConvert(buffer, read);
      }

    if (-1 > n)
      {
	return -1;
      }
    else
      {
	return n;
      }
  }

	
  text_iterator_c& text_iterator_c::operator ++ ()
  {
    poco_check_ptr (_pEncoding);
    poco_assert (_it != _end);
	
    unsigned char buffer[text_encoding_c::MAX_SEQUENCE_LENGTH];
    unsigned char* p = buffer;

    if (_it != _end)
      *p++ = *_it++;
    else
      *p++ = 0;

    int read = 1;
    int n = _pEncoding->sequenceLength(buffer, 1);

    while (-1 > n && (_end - _it) >= -n - read)
      {
	while (read < -n && _it != _end)
	  { 
	    *p++ = *_it++; 
	    read++; 
	  }
	n = _pEncoding->sequenceLength(buffer, read);
      }
    while (read < n && _it != _end)
      { 
	_it++; 
	read++; 
      }

    return *this;
  }


  text_iterator_c text_iterator_c::operator ++ (int)
  {
    text_iterator_c prev(*this);
    operator ++ ();
    return prev;
  }
 

  bool text_iterator_c::operator == (const text_iterator_c& it) const
  {
    return _it == it._it;
  }


  bool text_iterator_c::operator != (const text_iterator_c& it) const
  {
    return _it != it._it;
  }


  void swap (text_iterator_c& it1, text_iterator_c& it2)
  {
    it1.swap(it2);
  }


}
