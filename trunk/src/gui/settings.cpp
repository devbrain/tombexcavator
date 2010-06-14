#include <QtCore/QSettings>
#include <QtCore/QVariant>

#include "gui/settings.hpp"

struct settings_impl_s
{
  settings_impl_s ();

  void read ();
  void write ();

  const QString c_org;
  const QString c_app;

  QSettings m_settings;
  // fields
  QString   m_providers_path;
};

settings_impl_s::settings_impl_s ()
  : c_org ("Archeology Inc"),
    c_app ("tomb_excavator"),
    m_settings (c_org, c_app)
{
  
}
// --------------------------------------------------------
void settings_impl_s::read ()
{
  m_settings.beginGroup ("general");
  m_providers_path = m_settings.value ("providers_path", "").toString ();
  m_settings.endGroup   ();
}
// --------------------------------------------------------
void settings_impl_s::write ()
{
  m_settings.beginGroup ("general");
  m_settings.setValue   ("providers_path", m_providers_path);
  m_settings.endGroup   ();
}
// ========================================================
settings_c* settings_c::s_instance = 0;
// --------------------------------------------------------
settings_c* settings_c::instance ()
{
  if (!s_instance)
    {
      s_instance = new settings_c;
    }
  return s_instance;
}
// --------------------------------------------------------
settings_c::settings_c ()
{
  m_pimpl = new settings_impl_s;
  m_pimpl->read ();
}
// --------------------------------------------------------
QString settings_c::providers_path () const
{
  return m_pimpl->m_providers_path;
}
// --------------------------------------------------------
void settings_c::providers_path (const QString& path)
{
  m_pimpl->m_providers_path = path;
}
// --------------------------------------------------------
void settings_c::write ()
{
  m_pimpl->write ();
}
