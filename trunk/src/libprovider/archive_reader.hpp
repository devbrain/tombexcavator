#ifndef __LIBPROVIDER_ARCHIVE_READER_HPP__
#define __LIBPROVIDER_ARCHIVE_READER_HPP__

#include <string>
#include "libprovider/provider_export.hpp"
#include "abl/types/types.hpp"
#include "abl/fs/file.hpp"
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
    ssize_t _read (char* buff, std::size_t size);
    ssize_t _read (int8_t& v);
    ssize_t _read (uint8_t& v);
    ssize_t _read (int16_t& v);
    ssize_t _read (uint16_t& v);
    ssize_t _read (int32_t& v);
    ssize_t _read (uint32_t& v);

    file_size_t _file_size () const;
    file_size_t _seek (file_size_t pos);
    file_size_t _offset (); 
    
    virtual bool _get_next_resource (std::string& name, file_size_t& size, file_size_t& offset) = 0;
  private:
    archive_reader_impl_s* m_pimpl;
  };
}


#endif
