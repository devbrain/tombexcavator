#ifndef __LIBVFS_EXTRACTOR_VISITOR_HPP__
#define __LIBVFS_EXTRACTOR_VISITOR_HPP__

#include "libvfs/visitor.hpp"

namespace vfs
{
  class fs_dir_c;
  class fs_file_c;

  struct extractor_visitor_impl_s;

  class extractor_visitor_c : public visitor_c
  {
  public:
    extractor_visitor_c (const abl::path_c& root);
    ~extractor_visitor_c ();
    virtual bool visit (fs_object_c* object, const abl::path_c& path);
  protected:
    virtual bool _handle_dir  (fs_dir_c* dir, const abl::path_c& path) = 0;
    virtual bool _handle_file (fs_file_c* file, const abl::path_c& path) = 0;
  private:
    extractor_visitor_impl_s* m_pimpl;
  };
}

#endif
