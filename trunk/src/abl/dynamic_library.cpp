#include "abl/dynamic_library.hpp"
#include "abl/platform/dl/dl.h"
#include "abl/exception.hpp"
#include "abl/fs/path.hpp"
#include "thirdparty/predef/predef.h"

namespace abl 
{
  struct dynamic_library_impl_s
  {
    std::string      m_name;
    shared_object_s* m_handle;
  };
  // ==============================================================
  void* dynamic_library_c::symbol (const std::string& name)
  {
    return shared_object_get_symbol (m_pimpl->m_handle, name.c_str ());
  }
  // --------------------------------------------------------------
  std::string dynamic_library_c::name () const
  {
    return m_pimpl->m_name;
  }
  // --------------------------------------------------------------
  void dynamic_library_c::unload ()
  {
    this->release ();
  }
  // --------------------------------------------------------------
  dynamic_library_c::~dynamic_library_c ()
  {
    shared_object_close (m_pimpl->m_handle);
    delete m_pimpl;
  }
  // --------------------------------------------------------------
  dynamic_library_c::dynamic_library_c (const std::string& dir, const std::string& name)
  {
#if defined(PREDEF_OS_WINDOWS)
    const std::string prefix = "";
    const std::string suffix = ".dll";
#else
    const std::string prefix = "lib";
    const std::string suffix = ".so";
#endif
    path_c pth (path_c (dir), prefix + name + suffix);
    shared_object_s* handle = shared_object_open (pth.to_string ().c_str ());
    if (!handle)
      {
	throw library_load_exception_c ("Failed to load library", pth.to_string ());
      }
    m_pimpl = new dynamic_library_impl_s;
    m_pimpl->m_name = name;
    m_pimpl->m_handle = handle;
  }
  // --------------------------------------------------------------
  void dynamic_library_c::_duplicate ()
  {
    this->duplicate ();
  }
}
