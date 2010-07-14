#include "libvfs/for_each.hpp"
#include "libvfs/fs.hpp"
#include "libvfs/fs_object.hpp"
#include "libvfs/fs_dir.hpp"
#include "libvfs/visitor.hpp"

#include "abl/fs/path.hpp"

namespace vfs 
{
  bool for_each (fs_c* fs, visitor_c* visitor)
  {
    if (!fs)
      {
	return false;
      }
    abl::path_c pth;
    return for_each (fs->root_object (), pth, visitor);
  }
  // ------------------------------------------------------------------------
  bool for_each (fs_object_c* object, const abl::path_c& pth, visitor_c* visitor)
  {
    if (!visitor)
      {
	return false;
      }
    if (object->type () == fs_object_c::eFILE)
      {
	return visitor->visit (object, pth);
      }
    fs_dir_c* dir = dynamic_cast <fs_dir_c*> (object);
    visitor->visit (dir, pth);
    std::size_t files = dir->size ();
    for (std::size_t i=0; i<files; i++)
      {
	std::string name;
	inode_num_t ino;
	if (!dir->read (i, name, ino))
	  {
	    return false;
	  }
	fs_object_c* child = dir->load_object (ino);
	bool rc = for_each (child, abl::path_c (pth, name), visitor);
	if (!rc)
	  {
	    return false;
	  }
      }
    return true;
  }
}

