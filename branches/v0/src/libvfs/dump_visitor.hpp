#ifndef __LIBVFS_DUMP_VISITOR_HPP__
#define __LIBVFS_DUMP_VISITOR_HPP__

#include "libvfs/vfs_export.hpp"
#include "libvfs/extractor_visitor.hpp"

namespace vfs
{
  class VFS_API dump_visitor_c : public extractor_visitor_c
  {
  public:
    dump_visitor_c (const abl::path_c& root);
    ~dump_visitor_c ();
    virtual std::string describe () const;
  protected:
    virtual bool _handle_dir  (fs_dir_c* dir, const abl::path_c& path);
    virtual bool _handle_file (fs_file_c* file, const abl::path_c& path);
  };
}

#endif
