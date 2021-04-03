#ifndef __DUNE2_DIRECTORY_HPP__
#define __DUNE2_DIRECTORY_HPP__

#include <string>

#include "providers/westwood/dune2/dune2_export.hpp"

#include "libvfs/fs_dir.hpp"
#include "libvfs/inode_defs.hpp"
#include "libprovider/simple_dir.hpp"

struct directory_impl_s;

class DUNE2_API directory_c : public provider::simple_dir_c
{
public:
  directory_c (vfs::fs_c* owner, 
	       vfs::inode_num_t parent_inode,
	       const vfs::inode_num_t self_inode);
  ~directory_c ();
  
  bool open (const char* path);
  
  virtual vfs::inode_num_t    inode_num () const;
private:
  directory_impl_s* m_pimpl;
};


#endif
