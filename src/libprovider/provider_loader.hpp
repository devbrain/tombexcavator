#ifndef __LIBPROVIDER_PROVIDER_LOADER_HPP__
#define __LIBPROVIDER_PROVIDER_LOADER_HPP__

#include <string>

#include "abl/fs/path.hpp"
#include "libprovider/provider_export.hpp"

namespace provider
{
  class abstract_provider_c;
  struct loader_impl_s;

  class LIBPROVIDER_API loader_c
  {
  public:
    static loader_c* instance ();
    
    abstract_provider_c* load (const abl::path_c& so_path);
  private:
    static loader_c* s_instance;
  private:
    loader_c ();
    ~loader_c ();
  private:
    loader_impl_s* m_pimpl;
  };
  
} // ns provider

#endif
