#ifndef __LIBVFS_FS_OBJECT_HPP__
#define __LIBVFS_FS_OBJECT_HPP__

#include "libvfs/util/types.hpp"

#include "libvfs/inode_defs.hpp"

namespace vfs
{
  class fs_object_c
  {
  public:
    enum fs_object_type
      {
	eDIRECTORY,
	eFILE
      };
  public:
    fs_object_c ();
    virtual ~fs_object_c ();

    virtual inode_num_t    inode_num () const = 0;
    virtual inode_num_t    parent_inode_num () const = 0;
    
    virtual std::size_t    size      () const = 0;
    
    virtual fs_object_type type () const = 0;
    
  };
}

#endif
