#include "providers/westwood/libwestwood/pak_file.hpp"

namespace westwood 
{
  pak_file_c::pak_file_c () 
    : m_last_offset (0)
  {
  }
  // ------------------------------------------------------------------------
  bool pak_file_c::_get_next_resource (std::string& name, 
				       file_size_t& size, 
				       file_size_t& offset)
  {
    uint32_t offs;
    this->_read (offs);
    offset = offs;
    if (offset >= this->_file_size ())
      {
	return false;
      }
    if (offset == 0)
      {
	return false;
      }
    if (offset <= m_last_offset)
      {
	return false;
      }
    size = m_last_offset - offset;
    name = "";
    while (true)
      {
	int8_t ch;
	this->_read (ch);
	if (ch == 0)
	  {
	    break;
	  }
	name += ch;
      }
    m_last_offset = offset;
    return true;
  }
}
