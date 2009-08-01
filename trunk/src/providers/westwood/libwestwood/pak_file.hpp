#ifndef __LIBWESTWOOD_PAK_FILE_HPP__
#define __LIBWESTWOOD_PAK_FILE_HPP__

#include "libprovider/archive_reader.hpp"

namespace westwood
{
  
  class pak_file_c : public provider::archive_reader_c
  {
  public:
    pak_file_c ();
  private:
    virtual bool _get_next_resource (std::string& name, util::file_size_t& size, util::file_size_t& offset);
  private:
    util::file_size_t m_last_offset;
  };
}


#endif
