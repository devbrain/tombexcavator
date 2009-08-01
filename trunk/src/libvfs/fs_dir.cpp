#include "libvfs/fs_dir.hpp"

namespace vfs 
{
  fs_object_c::fs_object_type fs_dir_c::type () const
  {
    return fs_object_c::eDIRECTORY;
  }
}
