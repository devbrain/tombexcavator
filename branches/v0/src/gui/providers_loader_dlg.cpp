#include <QtCore/QDir>
#include <QtCore/QDebug>
#include <QtCore/QLibrary>

#include <QtGui/QTableWidgetItem>

#include "gui/providers_loader_dlg.hpp"
#include "gui/ui_providers_loader.h"
#include "gui/settings.hpp"
#include "gui/providers_registry.hpp"

#include "libprovider/provider_loader.hpp"
#include "libprovider/abstract_provider.hpp"

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
      QString qt_so_path = dir.absoluteFilePath (*i);
      const char* so_path = qt_so_path.toUtf8 ().constData ();
      provider::abstract_provider_c* ap = 
	provider::loader_c::instance ()->load (so_path);
      if (ap)
	{
	  providers_registry_c::instance ()->add_provider (qt_so_path, ap);
	  QString str_desc (ap->description ().c_str ());
	  QTableWidgetItem* prov = new QTableWidgetItem (*i);
	  QTableWidgetItem* desc = new QTableWidgetItem (str_desc);
	  m_ui->table->setRowCount (row+1);
	  m_ui->table->setItem (row, 0, prov);
	  m_ui->table->setItem (row, 1, desc);
	  row ++;
	}
    }
  m_ui->pushButton->setEnabled (true);
}
