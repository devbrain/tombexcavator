#ifndef __LIBVFS_FS_OBJECT_HPP__
#define __LIBVFS_FS_OBJECT_HPP__

#include <string>

#include "abl/types/types.hpp"

#include "libvfs/inode_defs.hpp"
#include "libvfs/vfs_export.hpp"

namespace vfs
{
  class fs_c;

  class VFS_API fs_object_c
  {
  public:
    enum fs_object_type
      {
	eDIRECTORY,
	eFILE
      };
  public:
    fs_object_c (fs_c* owner);
    virtual ~fs_object_c ();

    virtual inode_num_t    inode_num () const = 0;
    virtual inode_num_t    parent_inode_num () const = 0;
    
    virtual std::size_t    size      () const = 0;
    
    virtual fs_object_type type () const = 0;
    fs_c*                  owner ();
    const fs_c*            owner () const;
    virtual std::string    name () const = 0;
  private:
    fs_c* m_owner;
    
  };
}

#endif
