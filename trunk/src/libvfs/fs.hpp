#ifndef __LIBFS_FS_HPP__
#define __LIBFS_FS_HPP__

#include "libvfs/inode_defs.hpp"
#include "libvfs/vfs_export.hpp"
#include "abl/util/non_copyable.hpp"

namespace vfs
{
  
  class fs_object_c;
  
  class VFS_API fs_c : public util::non_copyable_c
  {
  public:
    fs_c ();
    virtual ~fs_c ();
    
    virtual bool         open (const char* path) = 0;
    virtual inode_num_t  root_inode_num () const = 0; 
    virtual fs_object_c* load_object (inode_num_t ino) = 0;
    virtual inode_num_t  new_inode_num () const = 0;
  };
}


#endif
