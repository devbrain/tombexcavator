#ifndef __LIBROVIDER_RESOURCE_HPP__
#define __LIBROVIDER_RESOURCE_HPP__

#include <string>
#include "abl/types/types.hpp"
#include "abl/fs/file.hpp"
#include "libprovider/provider_export.hpp"

namespace provider
{
  class archive_reader_c;
  class LIBPROVIDER_API resource_c
  {
    friend class archive_reader_c;
  public:
    resource_c ();

    std::string name () const;
    file_size_t size () const;
    file_size_t offset () const;

    bool valid () const;
  private:
    resource_c (const std::string& name, file_size_t size, file_size_t offset);
  private:
    std::string m_name;
    file_size_t m_size;
    file_size_t m_offset;
    bool        m_valid;
  };
}


#endif
