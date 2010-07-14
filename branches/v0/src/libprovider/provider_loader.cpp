#include "libprovider/provider_loader.hpp"
#include "libprovider/abstract_provider.hpp"

#include "abl/dynamic_loader.hpp"
#include "abl/dynamic_library.hpp"

namespace provider 
{
  loader_c* loader_c::s_instance = 0;
  // =======================================================
  struct loader_impl_s
  {
    abl::dynamic_loader_c m_dl;
  };
  
  loader_c* loader_c::instance ()
  {
    if (!s_instance)
      {
	s_instance = new loader_c;
      }
    return s_instance;
  }
  // ------------------------------------------------------------------
  abstract_provider_c* loader_c::load (const abl::path_c& so_path)
  {
    abl::dynamic_library_c* dl = m_pimpl->m_dl.load_from_path (so_path);
    if (!dl)
      {
	return 0;
      }
    provider_register_func_t reg_functor = 
      (provider_register_func_t) dl->symbol ("internal_provider_register_");
    if (!reg_functor)
      {
	dl->unload ();
	return 0;
      }
    provider_unregister_func_t unreg_functor = 
      (provider_unregister_func_t)dl->symbol ("internal_provider_unregister_");
    
    if (!unreg_functor)
      {
	dl->unload ();
	return 0;
      }
    abstract_provider_c* abstract_provider = reg_functor ();
    if (!abstract_provider)
      {
	dl->unload ();
	return 0;
      }
    abstract_provider->m_dl = dl;
    abstract_provider->m_destroy_functor = unreg_functor;
    
    return abstract_provider;
  }
  // ------------------------------------------------------------------
  loader_c::loader_c ()
  {
    m_pimpl = new loader_impl_s;
  }
  // ------------------------------------------------------------------
  loader_c::~loader_c ()
  {
    delete m_pimpl;
  }
} // ns provider
