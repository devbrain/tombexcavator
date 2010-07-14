#ifndef __LIBVFS_FS_HPP__
#define __LIBVFS_FS_HPP__

#include "libvfs/vfs_export.hpp"
#include "libvfs/inode_defs.hpp"
#include "abl/util/non_copyable.hpp"

namespace vfs
{
  
  class fs_object_c;
  
  class VFS_API fs_c : public util::non_copyable_c
  {
  public:
    fs_c ();
    virtual ~fs_c ();
    
    virtual bool         open           (const char* path) = 0;
    virtual fs_object_c* root_object    () = 0;
    virtual inode_num_t  new_inode_num  () const = 0;
  };
}


#endif
