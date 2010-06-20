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
  abstract_provider_c::~abstract_provider_c ()
  {
    if (m_destroy_functor)
      {
	m_destroy_functor (this);
      }
    if (m_dl)
      {
	m_dl->unload ();
      }
  }
} // ns provider
