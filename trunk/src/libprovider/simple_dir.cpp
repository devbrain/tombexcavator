#include <map>
#include "libvfs/fs_object.hpp"
#include "libvfs/fs.hpp"
#include "libprovider/simple_dir.hpp"


typedef std::map <vfs::inode_num_t, vfs::fs_object_c*> fs_map_t;
typedef fs_map_t::iterator                             fs_iterator_t;
typedef fs_map_t::const_iterator                       fs_const_iterator_t;
typedef fs_map_t::value_type                           fs_val_t;

typedef std::map <std::string, vfs::inode_num_t> nm_map_t;
typedef nm_map_t::iterator                       nm_iterator_t;
typedef nm_map_t::const_iterator                 const_nm_iterator_t;
typedef nm_map_t::value_type                     nm_val_t;

typedef std::map <std::size_t, vfs::inode_num_t> sz_map_t;
typedef sz_map_t::iterator                       sz_iterator_t;
typedef sz_map_t::const_iterator                 const_sz_iterator_t;
typedef sz_map_t::value_type                     sz_val_t;

typedef std::map <vfs::inode_num_t, std::string> rn_map_t;
typedef rn_map_t::iterator                       rn_iterator_t;
typedef rn_map_t::const_iterator                 const_rn_iterator_t;
typedef rn_map_t::value_type                     rn_val_t;


namespace provider 
{
  struct simple_dir_impl_s
  {
    ~simple_dir_impl_s ()
    {
      for (fs_iterator_t i = m_fs_map.begin (); i != m_fs_map.end (); i++)
	{
	  vfs::fs_object_c* victim = i->second;
	  delete victim;
	}
    }

    vfs::inode_num_t m_parent;

    fs_map_t         m_fs_map;
    nm_map_t         m_nm_map;
    sz_map_t         m_sz_map;
    rn_map_t         m_rn_map;
  };
  // ======================================================================
  simple_dir_c::simple_dir_c (vfs::fs_c* owner, vfs::inode_num_t parent_ino)
    : vfs::fs_dir_c (owner)
  {
    m_pimpl = new simple_dir_impl_s;
    m_pimpl->m_parent = parent_ino;
  }
  // -----------------------------------------------------------------------
  simple_dir_c::~simple_dir_c ()
  {
    delete m_pimpl;
  }
  // -----------------------------------------------------------------------
  vfs::fs_object_c* simple_dir_c::load_object (vfs::inode_num_t ino)
  {
    fs_iterator_t i = m_pimpl->m_fs_map.find (ino);
    if (i == m_pimpl->m_fs_map.end ())
      {
	return 0;
      }
    return i->second;
  }
  // -----------------------------------------------------------------------
  vfs::inode_num_t simple_dir_c::parent_inode_num () const
  {
    return m_pimpl->m_parent;
  }
  // -----------------------------------------------------------------------
  std::size_t simple_dir_c::size () const
  {
    return m_pimpl->m_fs_map.size ();
  }
  // -----------------------------------------------------------------------
  std::size_t simple_dir_c::read (std::size_t       entry_num, 
				  std::string&      name,
				  vfs::inode_num_t& inode_num)
  {
    inode_num = vfs::BAD_INODE_NUM;

    sz_iterator_t i = m_pimpl->m_sz_map.find (entry_num);
    if (i == m_pimpl->m_sz_map.end ())
      {
	return 0;
      }

    inode_num = i->second;
    
    rn_iterator_t j = m_pimpl->m_rn_map.find (inode_num);

    name = j->second;
    return 1;
  }
  // -----------------------------------------------------------------------
  vfs::inode_num_t simple_dir_c::lookup (const std::string& name) const
  {
    const_nm_iterator_t i = m_pimpl->m_nm_map.find (name);
    if (i == m_pimpl->m_nm_map.end ())
      {
	return vfs::BAD_INODE_NUM;
      }
    return i->second;
  }
  // -----------------------------------------------------------------------
  bool simple_dir_c::_add_fs_object (vfs::fs_object_c* fs_obj, 
				     const std::string& name)
  {
    if (fs_obj)
      {
	return false;
      }
    if (vfs::BAD_INODE_NUM != lookup (name))
      {
	return false;
      }
    vfs::inode_num_t ino = fs_obj->inode_num ();
    std::size_t      sz  = m_pimpl->m_fs_map.size ();
    m_pimpl->m_fs_map.insert (fs_val_t (ino , fs_obj));
    m_pimpl->m_nm_map.insert (nm_val_t (name, ino));
    m_pimpl->m_sz_map.insert (sz_val_t (sz  , ino));
    m_pimpl->m_rn_map.insert (rn_val_t (ino , name));
    return true;
  }
}
