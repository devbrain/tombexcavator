#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QLibrary>

#include <QtGui/QTableWidgetItem>

#include "gui/providers_loader_dlg.hpp"
#include "gui/ui_providers_loader.h"
#include "gui/settings.hpp"

providers_loader_dlg_c::providers_loader_dlg_c (QWidget* parent)
  : QDialog (parent)
{
  m_ui = new Ui::providers_loader;
  m_ui->setupUi (this);
  QString path = settings_c::instance ()->providers_path ();
  m_ui->label->setText (path);

  QStringList filter;
#if defined (Q_OS_UNIX)
  filter << QString ("lib*.so");
#elif defined (Q_OS_WINDOWS)
  filter << QString ("*.dll");
#else
#error "Specify shared object filter";
#endif

  QDir dir (path);
  
  QStringList providers = dir.entryList (filter, 
					 QDir::Files, 
					 QDir::IgnoreCase);

  int row = 0;
  for (QStringList::iterator i = providers.begin ();
       i != providers.end (); i++)
    {
      QLibrary lib (dir.absoluteFilePath (*i));
      if (lib.load ())
	{
	  void* d = lib.resolve ("internal_vfs_register_fs");
	  if (d)
	    {
	      QTableWidgetItem* prov = new QTableWidgetItem (*i);
	      QTableWidgetItem* desc = new QTableWidgetItem ("Zopa");
	      m_ui->table->setRowCount (row+1);
	      m_ui->table->setItem (row, 0, prov);
	      m_ui->table->setItem (row, 1, desc);
	      row ++;
	    }
	}
    }
  m_ui->pushButton->setEnabled (true);
}
