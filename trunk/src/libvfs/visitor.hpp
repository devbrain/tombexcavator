#ifndef __LIBVFS_VISITOR_HPP__
#define __LIBVFS_VISITOR_HPP__

#include <string>
#include "abl/util/non_copyable.hpp"

namespace abl
{
  class path_c;
}

namespace vfs
{
  class fs_object_c;

  class visitor_c : public util::non_copyable_c
  {
  public:
    visitor_c ();
    virtual ~visitor_c ();
    virtual std::string describe () const = 0;
    virtual bool visit (fs_object_c* object, const abl::path_c& path) = 0;
  };
}


#endif
