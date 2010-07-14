#include <QtGui/QMessageBox>
#include <QtGui/QApplication>
#include <QtGui/QCloseEvent>
#include <QtGui/QFileDialog>
#include <QtGui/QTreeWidget>

#include "gui/main_window.hpp"
#include "gui/providers_properties_dlg.hpp"
#include "gui/providers_registry.hpp"
#include "gui/settings.hpp"

#include "gui/models/vfs_model.hpp"

#include "libprovider/abstract_provider.hpp"
#include "libvfs/fs.hpp"

#include "gui/ui_main_widget.h"
// ==========================================================
static bool exit_question (QWidget* parent)
{
  int ret = QMessageBox::question (parent,
				   "Are you sure?.",
				   "Do you want to quit?",
				   QMessageBox::Ok | QMessageBox::Cancel,
				   QMessageBox::Ok);
  return (ret == QMessageBox::Ok);
}
// ==========================================================
main_window_c::main_window_c (QWidget* parent)
  : QMainWindow (parent)
{
  m_ui = new Ui::MainWindow;
  m_ui->setupUi (this);

  m_prov_props = new providers_properties_dlg_c (this, false);
  m_prov_props->set_caption ("Providers Properties");

  m_prov_chooser = new providers_properties_dlg_c (this, true);
  m_prov_chooser->set_caption ("Known Providers");

  this->_setup_actions ();
}
// ----------------------------------------------------------
void main_window_c::_setup_actions ()
{
  connect (m_ui->actionQuit, SIGNAL (triggered ()),
	   this            , SLOT   (_request_exit ())); 

  connect (m_ui->actionShow_Providers, SIGNAL (triggered ()),
	   this                      , SLOT   (_show_providers_props ()));

  connect (m_ui->actionMount_FS, SIGNAL (triggered ()),
	   this                , SLOT   (_mount_fs ()));
}
// ----------------------------------------------------------
void main_window_c::_add_fs (size_t prov_id)
{
  provider::abstract_provider_c* ap = 
    providers_registry_c::instance ()->abstract_provider (prov_id);
  QString prov_name (ap->name ().c_str ());
  fs_map_t::iterator i = m_fs_map.find (prov_id);
  if (i != m_fs_map.end ())
    {
      return;
    }
  vfs::fs_c* fs = providers_registry_c::instance ()->file_system (prov_id);
  QAbstractItemModel* model = new vfs_model_c (fs);
  QTreeView* tree = new QTreeView;
  tree->setModel(model);
  
  m_ui->fstab->addTab (tree, prov_name);
}
// ----------------------------------------------------------
void main_window_c::_request_exit ()
{
  if (exit_question (this))
    {
      qApp->quit ();
    }
}
// ----------------------------------------------------------
void main_window_c::_show_providers_props ()
{
  m_prov_props->exec ();
}
// ----------------------------------------------------------
void main_window_c::_mount_fs ()
{
  m_prov_chooser->exec ();
  size_t prov_id = m_prov_chooser->last_selected ();

  provider::abstract_provider_c* ap = 
    providers_registry_c::instance ()->abstract_provider (prov_id);
  if (!ap)
    {
      return;
    }
  QString prov_name (ap->name ().c_str ());

  provider_options_c po = 
    settings_c::instance ()->provider_options (prov_name);
  QString saved_load_path = po.path ();
  QString load_path = ".";
  if (!saved_load_path.isEmpty ())
    {
      load_path = saved_load_path;
    }

  QFileDialog fdlg (0, 
		    "Resources directory for " + prov_name, 
		    load_path,
		    "");
  fdlg.setFileMode (QFileDialog::Directory);
  if (!fdlg.exec ())
    {
      return;
    }
  vfs::fs_c* fs = providers_registry_c::instance ()->file_system (prov_id);
  if (!fs)
    {
      QMessageBox::critical (this, QString ("Error in ") + prov_name,
			     "Failed to allocate file system");
      return;
    }
  QString loaded_path = fdlg.directory ().path ();
  bool rc = fs->open (loaded_path.toUtf8 ().constData ());
  if (!rc)
    {
      QMessageBox::critical (this, QString ("Error in ") + prov_name,
			     "Failed to initialize file system:" +
			     loaded_path);
      return;
    }
  if (loaded_path != saved_load_path)
    {
      po.path (loaded_path);
      settings_c::instance ()->provider_options (prov_name, po);
    }
  this->_add_fs (prov_id);
}
// ----------------------------------------------------------
void main_window_c::closeEvent(QCloseEvent *event)
{

  if (exit_question (this))
    {
      event->accept ();
    }
  else
    {
      event->ignore ();
    }
}
