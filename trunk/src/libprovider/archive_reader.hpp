#ifndef __LIBPROVIDER_ARCHIVE_READER_HPP__
#define __LIBPROVIDER_ARCHIVE_READER_HPP__

#include <string>
#include "libprovider/provider_export.hpp"
#include "abl/types/types.hpp"
#include "abl/util/non_copyable.hpp"

namespace provider
{
  class resource_c;
  struct archive_reader_impl_s;

  class LIBPROVIDER_API archive_reader_c : public util::non_copyable_c
  {
  public:
    archive_reader_c ();
    virtual ~archive_reader_c ();
    
    bool        open (const char* path);
    std::size_t number_of_resources () const;
    
    bool describe_resource (std::size_t resource_number, resource_c& resource);
    char* load (const resource_c& resource);
    bool load (char* buff, const resource_c& resource);
  protected:
    util::ssize_t _read (char* buff, std::size_t size);
    util::ssize_t _read (util::sint8_t& v);
    util::ssize_t _read (util::uint8_t& v);
    util::ssize_t _read (util::sint16_t& v);
    util::ssize_t _read (util::uint16_t& v);
    util::ssize_t _read (util::sint32_t& v);
    util::ssize_t _read (util::uint32_t& v);

    util::file_size_t _file_size () const;
    util::file_size_t _seek (util::file_size_t pos);
    
    virtual bool _get_next_resource (std::string& name, util::file_size_t& size, util::file_size_t& offset) = 0;
  private:
    archive_reader_impl_s* m_pimpl;
  };
}


#endif
