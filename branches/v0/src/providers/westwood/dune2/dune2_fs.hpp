#ifndef __DUNE2_FS_HPP__
#define __DUNE2_FS_HPP__

#include "providers/westwood/dune2/dune2_export.hpp"
#include "libvfs/fs.hpp"

struct dune2_fs_impl_s;

class DUNE2_API dune2_fs_c : public vfs::fs_c
{
public:
  dune2_fs_c ();
  virtual ~dune2_fs_c ();
  virtual bool         open           (const char* path);
  virtual vfs::fs_object_c* root_object    ();
  virtual vfs::inode_num_t  new_inode_num  () const;
private:
  dune2_fs_impl_s* m_pimpl;
};


#endif
