#include "libprovider/abstract_provider.hpp"
#include "abl/dynamic_library.hpp"

namespace provider 
{
  abstract_provider_c::abstract_provider_c ()
    : m_destroy_functor (0),
      m_dl              (0)
  {
  }
  // --------------------------------------------------------
  void abstract_provider_c::release ()
  {
    if (m_destroy_functor)
      {
	m_destroy_functor (this);
      }
  }
  // --------------------------------------------------------
  abstract_provider_c::~abstract_provider_c ()
  {
    if (m_dl)
      {
	m_dl->unload ();
      }
  }
} // ns provider
