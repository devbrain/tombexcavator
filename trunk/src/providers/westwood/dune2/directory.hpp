#ifndef __DUNE2_DIRECTORY_HPP__
#define __DUNE2_DIRECTORY_HPP__

#include <string>

#include "libvfs/fs_dir.hpp"

struct directory_impl_s;

class directory_c : vfs::fs_dir_c
{
public:
  directory_c (vfs::inode_num_t my_inode);
  ~directory_c ();
  
  bool open (const char* path);

  virtual vfs::inode_num_t    inode_num () const;
  virtual vfs::inode_num_t    parent_inode_num () const;
  virtual std::size_t         size      () const;
  virtual std::size_t read (std::size_t entry_num, std::string& name, inode_num_t& inode_num);
private:
  directory_impl_s* m_pimpl;
};


#endif
