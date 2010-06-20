#ifndef __LIBPROVIDER_ABSTRACT_PROVIDER_HPP__
#define __LIBPROVIDER_ABSTRACT_PROVIDER_HPP__

#include <string>

#include "libprovider/provider_export.hpp"

namespace provider
{
  class LIBPROVIDER_API abstract_provider_c;
}

typedef provider::abstract_provider_c* (*provider_register_func_t) ();
typedef void (*provider_unregister_func_t) (provider::abstract_provider_c*);

namespace abl
{
  class dynamic_library_c;
}

namespace vfs
{
  class fs_c;
} // ns vfs

namespace provider
{
  class loader_c;

  class LIBPROVIDER_API abstract_provider_c
  {
    friend class loader_c;
  public:
    abstract_provider_c ();
    virtual ~abstract_provider_c ();

    virtual const std::string name () const = 0;
    virtual const std::string description () const = 0;
    virtual void  version (int& major, int& minor) const = 0;

    virtual vfs::fs_c* get_vfs () = 0;
  private:
    provider_unregister_func_t m_destroy_functor;
    abl::dynamic_library_c*    m_dl;
  };
} // ns provider

// ==============================================================


#define REGISTER_PROVIDER(CLASS)					\
  extern "C" te__api_export__ provider::abstract_provider_c*		\
  internal_provider_register_ ()					\
  {									\
    return new CLASS;							\
  }									\
  extern "C" te__api_export__ void internal_provider_unregister_ (provider::abstract_provider_c* x) \
  {									\
    delete x;								\
  }
								  



#endif
