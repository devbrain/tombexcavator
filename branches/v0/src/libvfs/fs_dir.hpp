#ifndef __LIBVFS_FS_DIR_HPP__
#define __LIBVFS_FS_DIR_HPP__

#include <string>

#include "libvfs/fs_object.hpp"
#include "libvfs/vfs_export.hpp"

namespace vfs
{
  
  class fs_c;

  class VFS_API fs_dir_c : public fs_object_c
  {
  public:
    fs_dir_c (fs_c* owner);
    virtual fs_object_type type () const;
    virtual std::size_t read (std::size_t entry_num, 
			      std::string& name, 
			      inode_num_t& inode_num) = 0;
    virtual fs_object_c* load_object (inode_num_t ino) = 0;
    virtual vfs::inode_num_t   lookup (const std::string& name) const = 0;
  };
}

#endif
