#include "abl/dynamic_loader.hpp"
#include "abl/dynamic_library.hpp"
#include "abl/fs/path.hpp"

#include <map>

namespace abl
{
  typedef std::map <std::string, dynamic_library_c*> so_map_t;

  struct dynamic_loader_impl_s
  {
    std::string m_root;
    so_map_t    m_map;
  };
  // ----------------------------------------------------------------
  dynamic_loader_c::dynamic_loader_c (const std::string& so_dir)
  {
    m_pimpl = new dynamic_loader_impl_s;
    m_pimpl->m_root = so_dir;
  }
  // ----------------------------------------------------------------
  dynamic_loader_c::dynamic_loader_c ()
  {
    m_pimpl = new dynamic_loader_impl_s;
  }
  // ----------------------------------------------------------------
  dynamic_loader_c::~dynamic_loader_c ()
  {
    for (so_map_t::iterator i = m_pimpl->m_map.begin (); 
	 i != m_pimpl->m_map.end (); i++)
      {
	delete i->second;
      }
    delete m_pimpl;
  }
  // ----------------------------------------------------------------
  dynamic_library_c* dynamic_loader_c::load (const std::string& name)
  {
    return this->load (m_pimpl->m_root, name);
  }
  // ----------------------------------------------------------------
  dynamic_library_c* dynamic_loader_c::load (const std::string& so_dir, 
					     const std::string& name)
  {
    path_c id (so_dir, name);
    return load_from_path (id);
  }
  // ----------------------------------------------------------------
  dynamic_library_c* dynamic_loader_c::load_from_path (const path_c& pth)
  {
    const std::string so_path = pth.to_string ();
    so_map_t::iterator i = m_pimpl->m_map.find (so_path);
    if (i != m_pimpl->m_map.end ())
      {
	dynamic_library_c* res = i->second;
	res->_duplicate ();
	return res;
      }
    dynamic_library_c* res = new dynamic_library_c (so_path);
    m_pimpl->m_map.insert (so_map_t::value_type (so_path, res));
    return res;
  }
}
