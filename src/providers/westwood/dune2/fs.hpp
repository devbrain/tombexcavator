#ifndef __DUNE2_FS_HPP__
#define __DUNE2_FS_HPP__

#include "libvfs/fs.hpp"
#include "providers/westwood/dune2/dune2_export.hpp"


struct fs_impl_s;

class DUNE2_API fs_c : public vfs::fs_c
{
public:
  fs_c ();
  virtual ~fs_c ();
  
  virtual bool         open           (const char* path) = 0;
  virtual inode_num_t  root_inode_num () const = 0; 
  virtual fs_object_c* load_object    (inode_num_t ino) = 0;
  virtual inode_num_t  new_inode_num  () const = 0;
private:
  fs_impl_s* m_pimpl;
};

#endif
