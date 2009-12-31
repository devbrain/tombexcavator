#ifndef __DUNE2_DIRECTORY_HPP__
#define __DUNE2_DIRECTORY_HPP__

#include <string>

#include "libvfs/fs_dir.hpp"
#include "libvfs/inode_defs.hpp"

#include "providers/westwood/dune2/dune2_export.hpp"

struct directory_impl_s;

class DUNE2_API directory_c : vfs::fs_dir_c
{
public:
  directory_c (vfs::fs_c* owner, vfs::inode_num_t parent_inode);
  ~directory_c ();
  
  bool open (const char* path);
  
  virtual vfs::fs_object_c*   load_object (vfs::inode_num_t ino);
  virtual vfs::inode_num_t    inode_num () const;
  virtual vfs::inode_num_t    parent_inode_num () const;
  virtual std::size_t         size      () const;
  virtual std::size_t read (std::size_t entry_num, std::string& name, vfs::inode_num_t& inode_num);
private:
  directory_impl_s* m_pimpl;
};


#endif
