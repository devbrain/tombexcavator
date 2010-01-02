#include <map>
#include <vector>

#include "providers/westwood/dune2/directory.hpp"
#include "providers/westwood/dune2/file.hpp"
#include "providers/westwood/libwestwood/pak_file.hpp"

#include "libprovider/resource.hpp"
#include "libvfs/fs.hpp"

struct directory_impl_s
{
  directory_impl_s ();
  ~directory_impl_s ();
  
  westwood::pak_file_c* m_pak_file;
  vfs::inode_num_t      m_my_inode;
};
// ------------------------------------------------------------------------
directory_impl_s::directory_impl_s ()
{
  m_pak_file = new westwood::pak_file_c;
}
// ------------------------------------------------------------------------
directory_impl_s::~directory_impl_s ()
{
  delete m_pak_file;
}
// ========================================================================
directory_c::directory_c (vfs::fs_c* owner, 
			  vfs::inode_num_t parent_inode,
			  const std::string& name,
			  const vfs::inode_num_t self_inode)
  : provider::simple_dir_c (owner, parent_inode, name)
{
  m_pimpl = new directory_impl_s;
  m_pimpl->m_my_inode = self_inode;
}
// ------------------------------------------------------------------------
directory_c::~directory_c ()
{
  delete m_pimpl;
}
// ------------------------------------------------------------------------
bool directory_c::open (const char* path)
{
  if (!m_pimpl->m_pak_file->open (path))
    {
      return false;
    }
  
  const std::size_t res_num = m_pimpl->m_pak_file->number_of_resources ();
  for (std::size_t i=0; i<res_num; i++)
    {
      provider::resource_c resource;
      if (!m_pimpl->m_pak_file->describe_resource (i, resource))
	{
	  return false;
	}
      if (!resource.valid ())
	{
	  return false;
	}
      const vfs::inode_num_t ino = owner ()->new_inode_num ();
      file_c* file = new file_c (this->owner (), 
				 ino, 
				 this->inode_num (), 
				 m_pimpl->m_pak_file, 
				 resource);

      if (!this->_add_fs_object (file))
	{
	  return false;
	}
    }
  return true;
}
// ------------------------------------------------------------------------
vfs::inode_num_t directory_c::inode_num () const
{
  return m_pimpl->m_my_inode;
}
// ------------------------------------------------------------------------

