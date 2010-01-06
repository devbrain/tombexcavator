#ifndef __LIBVFS_PRINT_VISITOR_HPP__
#define __LIBVFS_PRINT_VISITOR_HPP__

#include "libvfs/vfs_export.hpp"
#include "libvfs/visitor.hpp"


namespace vfs
{
  class VFS_API print_visitor_c : public visitor_c
  {
    virtual std::string describe () const;
    virtual bool visit (fs_object_c* object, const abl::path_c& path);
  };
}

#endif
