#include <map>
#include <vector>

#include "providers/westwood/dune2/directory.hpp"
#include "providers/westwood/dune2/file.hpp"
#include "providers/westwood/libwestwood/pak_file.hpp"

#include "libprovider/resource.hpp"
#include "libvfs/fs.hpp"


// ========================================================================
typedef std::map <vfs::inode_num_t, provider::resource_c> res_map_t;
typedef res_map_t::iterator        iterator_t;
typedef res_map_t::const_iterator  const_iterator_t;
typedef res_map_t::value_type      val_t;
// ========================================================================
struct directory_impl_s
{
  directory_impl_s (vfs::inode_num_t parent_inode, vfs::inode_num_t my_inode);
  ~directory_impl_s ();
  
  westwood::pak_file_c* m_pak_file;
  vfs::inode_num_t      m_parent_inode;
  vfs::inode_num_t      m_my_inode;
  

  std::vector <vfs::inode_num_t> m_inodes;
  res_map_t                      m_resources; 

  std::string                    m_name;
};
// ------------------------------------------------------------------------
directory_impl_s::directory_impl_s (vfs::inode_num_t parent_inode, 
				    vfs::inode_num_t my_inode)
  : m_parent_inode (parent_inode),
    m_my_inode     (my_inode)
{
  m_pak_file = new westwood::pak_file_c;
}
// ------------------------------------------------------------------------
directory_impl_s::~directory_impl_s ()
{
  delete m_pak_file;
}
// ========================================================================
directory_c::directory_c (vfs::fs_c* owner, vfs::inode_num_t parent_inode)
  : vfs::fs_dir_c (owner)
{
  m_pimpl = new directory_impl_s (parent_inode, owner->new_inode_num ());
}
// ------------------------------------------------------------------------
directory_c::~directory_c ()
{
  delete m_pimpl;
}
// ------------------------------------------------------------------------
bool directory_c::open (const char* path, const std::string& name)
{
  m_pimpl->m_name = name;
  if (!m_pimpl->m_pak_file->open (path))
    {
      return false;
    }
  
  const std::size_t res_num = m_pimpl->m_pak_file->number_of_resources ();
  m_pimpl->m_inodes.reserve (res_num);
  for (std::size_t i=0; i<res_num; i++)
    {
      vfs::inode_num_t rinode = this->owner ()->new_inode_num ();
      provider::resource_c resource;
      if (!m_pimpl->m_pak_file->describe_resource (i, resource))
	{
	  return false;
	}
      if (!resource.valid ())
	{
	  return false;
	}
      
      m_pimpl->m_inodes.push_back (rinode);
      m_pimpl->m_resources.insert (val_t (rinode, resource));
    }
  return true;
}
// ------------------------------------------------------------------------
vfs::inode_num_t directory_c::inode_num () const
{
  return m_pimpl->m_my_inode;
}
// ------------------------------------------------------------------------
vfs::inode_num_t directory_c::parent_inode_num () const
{
  return m_pimpl->m_parent_inode;
}
// ------------------------------------------------------------------------
std::size_t directory_c::size () const
{
  return m_pimpl->m_inodes.size ();
}
// ------------------------------------------------------------------------
std::string directory_c::name () const
{
  return m_pimpl->m_name;
}
// ------------------------------------------------------------------------
std::size_t directory_c::read (std::size_t entry_num, std::string& name, 
			       vfs::inode_num_t& inode_num)
{
  inode_num = vfs::BAD_INODE_NUM;
  if (entry_num >= size ())
    {
      return 0;
    }
  vfs::inode_num_t rinode = m_pimpl->m_inodes [entry_num];
  const_iterator_t i = m_pimpl->m_resources.find (rinode);
  if (i == m_pimpl->m_resources.end ())
    {
      return 0;
    }
  name      = i->second.name ();
  inode_num = rinode;
  return 1;
}

// --------------------------------------------------------------------------
vfs::fs_object_c* directory_c::load_object (vfs::inode_num_t ino)
{
  const_iterator_t i = m_pimpl->m_resources.find (ino);
  if (i == m_pimpl->m_resources.end ())
    {
      return 0;
    }
  const provider::resource_c& res = i->second;
  return new file_c (this->owner (), ino, this->inode_num (), 
		     m_pimpl->m_pak_file, res);
}
