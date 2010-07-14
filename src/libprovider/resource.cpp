#include "libprovider/resource.hpp"

namespace provider 
{
  resource_c::resource_c ()
    : m_name   (""),
      m_size   (0),
      m_offset (0),
      m_valid  (false)
  {
  }
  // ----------------------------------------------------------------------------------------------
  resource_c::resource_c (const std::string& name, file_size_t size, file_size_t offset)
    : m_name   (name),
      m_size   (size),
      m_offset (offset),
      m_valid  (true)
  {
  }
  // ----------------------------------------------------------------------------------------------
  std::string resource_c::name () const
  {
    return m_name;
  }
  // ----------------------------------------------------------------------------------------------
  file_size_t resource_c::size () const
  {
    return m_size;
  }
  // ----------------------------------------------------------------------------------------------
  file_size_t resource_c::offset () const
  {
    return m_offset;
  }
  // ----------------------------------------------------------------------------------------------
  bool resource_c::valid () const
  {
    return m_valid;
  }
}
