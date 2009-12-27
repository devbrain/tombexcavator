#ifndef __LIBROVIDER_RESOURCE_HPP__
#define __LIBROVIDER_RESOURCE_HPP__

#include <string>
#include "abl/types/types.hpp"
#include "libprovider/provider_export.hpp"

namespace provider
{
  class archive_reader_c;
  class LIBPROVIDER_API resource_c
  {
    friend class archive_reader_c;
  public:
    resource_c ();

    std::string       name () const;
    util::file_size_t size () const;
    util::file_size_t offset () const;

    bool valid () const;
  private:
    resource_c (const std::string& name, util::file_size_t size, util::file_size_t offset);
  private:
    std::string       m_name;
    util::file_size_t m_size;
    util::file_size_t m_offset;
    bool              m_valid;
  };
}


#endif
