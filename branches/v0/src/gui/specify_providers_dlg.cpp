#include <QtGui/QFileDialog>
#include <QtCore/QDebug>

#include "gui/specify_providers_dlg.hpp"
#include "gui/ui_specify_providers.h"
#include "gui/settings.hpp"

specify_providers_dlg_c::specify_providers_dlg_c (QWidget* parent)
  : QDialog (parent)
{
  m_ui = new Ui::specify_providers;
  m_ui->setupUi (this);

  m_ui->edt_location->setText (settings_c::instance ()->providers_path ());

  connect (m_ui->btnBrowse, SIGNAL (clicked ()),
	   this           , SLOT   (_on_browse ()));
}
// --------------------------------------------------------------------
void specify_providers_dlg_c::_on_browse ()
{
  QFileDialog fdlg (0, 
		    "Providers Location", 
		    m_ui->edt_location->text (),
		    "");
  fdlg.setFileMode (QFileDialog::Directory);
  if (fdlg.exec ())
    {
      m_ui->edt_location->setText (fdlg.directory ().path ());
    }
}
// --------------------------------------------------------------------
void specify_providers_dlg_c::accept ()
{
  QString path = m_ui->edt_location->text ();
  settings_c::instance ()->providers_path (path);
  settings_c::instance ()->write ();
  QDialog::accept ();
}
// --------------------------------------------------------------------
void specify_providers_dlg_c::reject ()
{
  QDialog::reject ();
}
