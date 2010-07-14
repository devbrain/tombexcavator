#include <QtCore/QVector>

#include "gui/providers_registry.hpp"

#include "libprovider/abstract_provider.hpp"

struct provider_info_s
{
  provider_info_s (const QString& so_path, 
		   provider::abstract_provider_c* prov);
  ~provider_info_s ();
  vfs::fs_c* fs ();

  const QString                  m_so_path;
  provider::abstract_provider_c* m_provider;
  vfs::fs_c*                     m_fs;
};

provider_info_s::provider_info_s (const QString& so_path, 
				  provider::abstract_provider_c* prov)
  : m_so_path  (so_path),
    m_provider (prov),
    m_fs       (0)
{
}

provider_info_s::~provider_info_s ()
{
}

vfs::fs_c* provider_info_s::fs ()
{
  if (!m_fs && m_provider)
    {
      m_fs = m_provider->get_vfs ();
    }
  return m_fs;
}
// ==========================================================
typedef QVector <provider_info_s*> prov_vec_t;

struct providers_registry_impl_s
{
  prov_vec_t m_providers;
};
// ------------------------------------------------------------------
providers_registry_c* providers_registry_c::s_instance = 0;
// ------------------------------------------------------------------
providers_registry_c* providers_registry_c::instance ()
{
  if (!s_instance)
    {
      s_instance = new providers_registry_c;
    }
  return s_instance;
}
// ------------------------------------------------------------------
providers_registry_c::providers_registry_c ()
{
  m_pimpl = new providers_registry_impl_s;
}
// ------------------------------------------------------------------
providers_registry_c::~providers_registry_c ()
{
  delete m_pimpl;
}
// ------------------------------------------------------------------
void providers_registry_c::add_provider (const QString& so_path, 
					 provider::abstract_provider_c* prov)
{
  m_pimpl->m_providers.push_back (new provider_info_s (so_path, prov));
}
// ------------------------------------------------------------------
size_t  providers_registry_c::size () const
{
  return m_pimpl->m_providers.size ();
}
// ------------------------------------------------------------------
QString providers_registry_c::so_path (size_t idx) const
{
  QString result;
  if (idx < m_pimpl->m_providers.size ())
    {
      result = m_pimpl->m_providers.at (idx)->m_so_path;
    }
  return result;
}
// ------------------------------------------------------------------
provider::abstract_provider_c* 
providers_registry_c::abstract_provider (size_t idx) const
{
  provider::abstract_provider_c* result = 0;
  if (idx < m_pimpl->m_providers.size ())
    {
      result = m_pimpl->m_providers.at (idx)->m_provider;
    }
  return result;
}
// -----------------------------------------------------------------
vfs::fs_c* providers_registry_c::file_system (size_t idx) const
{
  vfs::fs_c* result = 0;
  if (idx < m_pimpl->m_providers.size ())
    {
      result = m_pimpl->m_providers.at (idx)->fs ();
    }
  return result;
}
