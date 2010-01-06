#ifndef __LIBVFS_FOR_EACH_HPP__
#define __LIBVFS_FOR_EACH_HPP__

#include "libvfs/vfs_export.hpp"

namespace abl
{
  class path_c;
}

namespace vfs
{
  class fs_c;
  class fs_object_c;
  class visitor_c;

  VFS_API bool for_each (fs_c* fs, visitor_c* visitor);
  VFS_API bool for_each (fs_object_c* object, const abl::path_c& name, visitor_c* visitor);
}


#endif
