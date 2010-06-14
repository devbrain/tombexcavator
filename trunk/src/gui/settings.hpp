#ifndef __GUI_SETTINGS_HPP__
#define __GUI_SETTINGS_HPP__

#include <QtCore/QString>

struct settings_impl_s;

class settings_c
{
public:
  static settings_c* instance ();

  QString providers_path () const;
  void    providers_path (const QString& path);
  
  void write ();
private:
  settings_c ();
private:
  static settings_c* s_instance;
private:
  settings_impl_s* m_pimpl;
};


#endif
