#include <QApplication>

#include "gui/settings.hpp"
#include "gui/specify_providers_dlg.hpp"
#include "gui/providers_loader_dlg.hpp"
#include "gui/main_window.hpp"

void load_settings ()
{
  specify_providers_dlg_c dlg (0);
  while (settings_c::instance ()->providers_path ().isEmpty ())
    {
      dlg.exec ();
    }
}

int main(int argc, char *argv[])
{
  QApplication app (argc, argv);
  
  load_settings ();

  providers_loader_dlg_c pl;

  pl.exec ();

  main_window_c main_window;

  main_window.show ();

  return app.exec();
}
