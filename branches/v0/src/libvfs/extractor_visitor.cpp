#include <stdio.h>
#include "libvfs/extractor_visitor.hpp"
#include "libvfs/fs_object.hpp"
#include "libvfs/fs_file.hpp"
#include "libvfs/fs_dir.hpp"

#include "abl/fs/path.hpp"

namespace vfs  
{
  struct extractor_visitor_impl_s
  {
    abl::path_c m_root;
  };
  // ============================================================================
  extractor_visitor_c::extractor_visitor_c (const abl::path_c& root)
  {
    m_pimpl = new extractor_visitor_impl_s;
    m_pimpl->m_root = root;
  }
  // ----------------------------------------------------------------------------
  extractor_visitor_c::~extractor_visitor_c ()
  {
    delete m_pimpl;
  }
  // ----------------------------------------------------------------------------
  bool extractor_visitor_c::visit (fs_object_c* object, const abl::path_c& path)
  {
    abl::path_c pth = m_pimpl->m_root;
    pth.append (path);
    if (object->type () == fs_object_c::eDIRECTORY)
      {
	pth.make_directory ();
	fs_dir_c* dir = dynamic_cast <fs_dir_c*> (object);
	return this->_handle_dir (dir, pth);
      }
    else
      {
	pth.make_file ();
	fs_file_c* fl = dynamic_cast <fs_file_c*> (object);
	return this->_handle_file (fl, pth);
      }
    return false;
  }
}
