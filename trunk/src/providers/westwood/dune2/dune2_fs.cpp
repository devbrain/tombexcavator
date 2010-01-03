#include "providers/westwood/dune2/dune2_fs.hpp"
#include "providers/westwood/dune2/directory.hpp"
#include "libprovider/simple_dir.hpp"

#include "libvfs/register.hpp"

#include "abl/fs/glob.hpp"
#include "abl/fs/path.hpp"
#include "abl/fs/directory_iterator.hpp"
#include "abl/string/utf8_string.hpp"


static const vfs::inode_num_t ROOT_INODE = 0;

class dune2_root_c : public provider::simple_dir_c
{
public:
  dune2_root_c (vfs::fs_c* owner);
  vfs::inode_num_t inode_num () const;

  bool add_child (const abl::path_c& root, const std::string& name);
};
// ----------------------------------------------------
dune2_root_c::dune2_root_c (vfs::fs_c* owner)
  : simple_dir_c (owner, vfs::BAD_INODE_NUM)
{
}
// ----------------------------------------------------
vfs::inode_num_t dune2_root_c::inode_num () const
{
  return ROOT_INODE;
}
// ----------------------------------------------------
bool dune2_root_c::add_child (const abl::path_c& root, 
			      const std::string& name)
{
  const abl::path_c pth (root, name);
  const std::string basename = abl::utf8_s::to_lower (pth.get_base_name ());

  directory_c* d = new directory_c (owner (), inode_num (), owner ()->new_inode_num ());
  bool rc = d->open (pth.to_string ().c_str ());
  if (!rc)
    {
      return false;
    }
  return this->_add_fs_object (d, basename);
}

// ==============================================================

struct dune2_fs_impl_s
{
  dune2_fs_impl_s  ();
  ~dune2_fs_impl_s ();

  vfs::inode_num_t m_inode;
  dune2_root_c*    m_root;
};

dune2_fs_impl_s::dune2_fs_impl_s ()
  : m_inode (ROOT_INODE),
    m_root  (0)
{
}
// -------------------------------------------------------------
dune2_fs_impl_s::~dune2_fs_impl_s ()
{
  if (m_root)
    {
      delete m_root;
    }
}
// =============================================================
dune2_fs_c::dune2_fs_c ()
{
  m_pimpl = new dune2_fs_impl_s;
}
// -------------------------------------------------------------
dune2_fs_c::~dune2_fs_c ()
{
  delete m_pimpl;
}
// -------------------------------------------------------------
bool dune2_fs_c::open (const char* path)
{
  m_pimpl->m_root = new dune2_root_c (this);

  abl::path_c root_path (path);
  abl::directory_iterator_c end;
  abl::directory_iterator_c itr (root_path);
  
  const int options = abl::glob_c::GLOB_CASELESS;
  abl::glob_c glob ("*.pak", options);
  
  while (itr != end)
    {
      abl::path_c pth = itr.path ();
      if (glob.match (pth.to_string ()))
	{
	  if (!m_pimpl->m_root->add_child (path, pth.get_file_name ()))
	    {
	      return false;
	    }
	}
      ++itr;
    }
  return true;
}
// -------------------------------------------------------------
vfs::fs_object_c* dune2_fs_c::root_object ()
{
  return m_pimpl->m_root;
}
// -------------------------------------------------------------
vfs::inode_num_t dune2_fs_c::new_inode_num  () const
{
  return m_pimpl->m_inode++;
}
// ============================================================
REGISTER_VFS (dune2_fs_c)
