#ifndef __GUI_SETTINGS_HPP__
#define __GUI_SETTINGS_HPP__

#include <QtCore/QString>

class QSettings;
class settings_c;

class provider_options_c
{
  friend class settings_c;
public:
  provider_options_c ();
  ~provider_options_c ();

  QString path () const;
  void    path (const QString& v);
private:
  void _write (QSettings& settings) const;
  void _read  (QSettings& settings);
private:
  QString m_path;
};

struct settings_impl_s;

class settings_c
{
public:
  static settings_c* instance ();

  QString providers_path () const;
  void    providers_path (const QString& path);

  provider_options_c provider_options (const QString& prov_name);

  void    provider_options (const QString& prov_name, 
			    const provider_options_c& options);
  
  void write ();
private:
  settings_c ();
private:
  static settings_c* s_instance;
private:
  settings_impl_s* m_pimpl;
};


#endif
