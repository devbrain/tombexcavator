#include "providers/westwood/dune2/file.hpp"
#include "providers/westwood/libwestwood/pak_file.hpp"
#include "libprovider/resource.hpp"

struct file_impl_s
{
  file_impl_s (vfs::inode_num_t      my_inode, vfs::inode_num_t            parent_inode, 
	       westwood::pak_file_c* pak_file, const provider::resource_c& resource);

  vfs::inode_num_t      m_my_inode;
  vfs::inode_num_t      m_parent_inode;
  westwood::pak_file_c* m_pak_file;
  provider::resource_c  m_resource;
};

file_impl_s::file_impl_s (vfs::inode_num_t      my_inode, vfs::inode_num_t            parent_inode, 
			  westwood::pak_file_c* pak_file, const provider::resource_c& resource)
  : m_my_inode     (my_inode),
    m_parent_inode (parent_inode),
    m_pak_file     (pak_file),
    m_resource     (resource)
{
}

// =========================================================================================================================

file_c::file_c (vfs::fs_c*            owner       , vfs::inode_num_t      my_inode,
		vfs::inode_num_t      parent_inode, westwood::pak_file_c* pak_file, 
		const provider::resource_c& resource)
  : vfs::fs_file_c (owner)
{
  m_pimpl = new file_impl_s (my_inode, parent_inode, pak_file, resource);
}
// ------------------------------------------------------------------------------------------------------------------------
file_c::~file_c ()
{
  delete m_pimpl;
}
// ------------------------------------------------------------------------------------------------------------------------
vfs::inode_num_t file_c::inode_num () const
{
  return m_pimpl->m_my_inode;
}
// ------------------------------------------------------------------------------------------------------------------------
vfs::inode_num_t file_c::parent_inode_num () const
{
  return m_pimpl->m_parent_inode;
}
// ------------------------------------------------------------------------------------------------------------------------
std::size_t file_c::size () const
{
  return m_pimpl->m_resource.size ();
}
// ------------------------------------------------------------------------------------------------------------------------
std::size_t file_c::read (char* buff, std::size_t size)
{
  if (size != this->size ())
    {
      return 0;
    }
  m_pimpl->m_pak_file->load (buff, m_pimpl->m_resource);
  return size;
}
