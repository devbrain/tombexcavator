#ifndef __DUNE2_FILE_HPP__
#define __DUNE2_FILE_HPP__

#include "libvfs/fs_file.hpp"

#include "providers/westwood/dune2/dune2_export.hpp"

namespace westwood
{
  class pak_file_c;
}

namespace provider
{
  class resource_c;
}

struct file_impl_s;

class DUNE2_API file_c : public vfs::fs_file_c
{
public:
  file_c (vfs::fs_c*            owner       , vfs::inode_num_t      my_inode,
	  vfs::inode_num_t      parent_inode, westwood::pak_file_c* pak_file, 
	  const provider::resource_c& resource);
  ~file_c ();
  
  virtual vfs::inode_num_t inode_num () const;
  virtual vfs::inode_num_t parent_inode_num () const;
    
  virtual std::size_t size () const;
  virtual std::size_t read (char* buff, std::size_t size);
  virtual std::string name () const;
private:
  file_impl_s* m_pimpl;

};


#endif
