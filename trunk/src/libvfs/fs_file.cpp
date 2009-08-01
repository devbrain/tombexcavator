#include "libvfs/fs_file.hpp"

namespace vfs 
{

  fs_object_c::fs_object_type fs_file_c::type () const 
  {
    return fs_object_c::eFILE;
  }

}
