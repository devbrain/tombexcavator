#ifndef __ABL_DYNAMIC_LIBRARY_HPP__
#define __ABL_DYNAMIC_LIBRARY_HPP__

#include <string>
#include "abl/abl_export.h"
#include "abl/object/ref_counted_object.hpp"

namespace abl
{
  class dynamic_loader_c;
  struct dynamic_library_impl_s;

  class ABL_API dynamic_library_c : private ref_counted_object_c
  {
    friend class dynamic_loader_c;
  public:
    void* symbol (const std::string& name);
    std::string name () const;
    void unload ();
  private:
    dynamic_library_c (const std::string& dir, const std::string& name);
    virtual ~dynamic_library_c ();
    void _duplicate ();
  private:
    dynamic_library_impl_s* m_pimpl;
  };
}


#endif
