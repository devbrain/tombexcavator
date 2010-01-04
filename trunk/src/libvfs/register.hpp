#ifndef __LIBVFS_REGISTER_HPP__
#define __LIBVFS_REGISTER_HPP__

#include "abl/export_def.h"
 
namespace vfs
{
  class fs_c;
}


typedef vfs::fs_c* (*vfs_register_func_t) ();

#define REGISTER_VFS(CLASS)						\
  extern "C" te__api_export__ vfs::fs_c* internal_vfs_register_fs ()	\
  {									\
    return new CLASS;							\
  }


#endif
