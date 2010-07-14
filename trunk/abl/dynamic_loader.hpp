#ifndef __ABL_DYNAMIC_LOADER_HPP__
#define __ABL_DYNAMIC_LOADER_HPP__

#include <string>
#include "abl/abl_export.h"

namespace abl
{
  class dynamic_library_c;
  struct dynamic_loader_impl_s;
  class path_c;

  class ABL_API dynamic_loader_c
  {
  public:
    dynamic_loader_c ();
    dynamic_loader_c (const std::string& so_dir);
    
    ~dynamic_loader_c ();
    
    dynamic_library_c* load (const std::string& name);
    dynamic_library_c* load (const std::string& so_dir, 
			     const std::string& name);
    dynamic_library_c* load_from_path (const path_c& so_path); 

  private:
    dynamic_loader_impl_s* m_pimpl;
  };

}

#endif

