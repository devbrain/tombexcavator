#include "providers/westwood/libwestwood/pak_file.hpp"

namespace westwood 
{
  pak_file_c::pak_file_c () 
    : m_last_offset (0)
  {
  }
  // ------------------------------------------------------------------------
  pak_file_c::~pak_file_c ()
  {
  }
  // ------------------------------------------------------------------------
  bool pak_file_c::_read_archive_header ()
  {
    return true;
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
    file_size_t current = this->_offset ();
    this->_read (offs);
    if (offs >= this->_file_size () || offs == 0 || name.empty ())
      {
	size = this->_file_size () - offset;
      }
    else
      {
	size = offs - offset;
      }
    this->_seek (current);
    return true;
  }
}
