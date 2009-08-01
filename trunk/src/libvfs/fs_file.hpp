#ifndef __LIBVFS_FS_FILE_HPP__
#define __LIBVFS_FS_FILE_HPP__

#include "libvfs/fs_object.hpp"

namespace vfs
{
  class fs_file_c : public fs_object_c
  {
  public:
    virtual fs_object_type type () const;
    virtual std::size_t read (char* buff, std::size_t size) = 0;
  };
}


#endif
