#ifndef __PROVIDERS_REGISTRY_HPP__
#define __PROVIDERS_REGISTRY_HPP__

#include <QtCore/QString>

namespace provider
{
  class abstract_provider_c;
};

namespace vfs
{
  class fs_c;
}

struct providers_registry_impl_s;

class providers_registry_c
{
public:
  static providers_registry_c* instance ();

  void add_provider (const QString& so_path, 
		     provider::abstract_provider_c* prov);
  size_t  size () const;
  QString so_path (size_t idx) const;
  provider::abstract_provider_c* abstract_provider (size_t idx) const;

  vfs::fs_c* file_system (size_t idx) const;
private:
  providers_registry_c ();
  ~providers_registry_c ();
private:
  static providers_registry_c* s_instance;
private:
  providers_registry_impl_s* m_pimpl;
};


#endif
