#include <QtGui/QListWidgetItem>

#include "gui/providers_properties_dlg.hpp"
#include "gui/providers_registry.hpp"
#include "libprovider/abstract_provider.hpp"

#include "gui/ui_providers_properties.h"

class props_item_c : public QListWidgetItem
{
public:
  props_item_c (provider::abstract_provider_c* ap,
		const QString&                 so_path,
		size_t                         prov_id)

    : QListWidgetItem (QString (ap->name ().c_str ())),
      m_provider (ap),
      m_so_path  (so_path),
      m_prov_id  (prov_id)
  {
  }
  provider::abstract_provider_c* abstract_provider () const
  {
    return m_provider;
  }
  const QString& so_path () const
  {
    return m_so_path;
  }
  size_t prov_id () const
  {
    return m_prov_id;
  }
private:
  provider::abstract_provider_c* m_provider;
  const QString                  m_so_path;
  const size_t                   m_prov_id;
};

// ======================================================================
providers_properties_dlg_c::providers_properties_dlg_c (QWidget* parent,
							bool dbl_click_accept)
  : QDialog (parent),
    m_dbl_click_accept (dbl_click_accept),
    m_last_selected (0)
{
  m_ui = new Ui::ProvidersPropsDlg;
  m_ui->setupUi (this);

  connect (m_ui->providersList, SIGNAL (itemClicked   (QListWidgetItem*)),
	   this               , SLOT   (_on_prov_item (QListWidgetItem*)));

  connect (m_ui->providersList, SIGNAL (itemDoubleClicked (QListWidgetItem*)),
	   this               , SLOT   (_on_prov_selected (QListWidgetItem*)));
  

  size_t total_providers = providers_registry_c::instance ()->size ();
  QListWidgetItem* first_item = 0;
  for (size_t i=0; i<total_providers; i++)
    {
      provider::abstract_provider_c* ap = 
	providers_registry_c::instance ()->abstract_provider (i);
      QString so_path = providers_registry_c::instance ()->so_path (i);
      if (ap)
	{
	  QListWidgetItem* item = new props_item_c (ap, so_path, i);
	  if (!first_item)
	    {
	      first_item = item;
	    }
	  m_ui->providersList->addItem (item);
	}
    }
  _on_prov_item (first_item);
}
// ------------------------------------------------------------------------
void providers_properties_dlg_c::set_caption (const QString& title)
{
  this->setWindowTitle (title);
}
// ------------------------------------------------------------------------
void providers_properties_dlg_c::_display_item (QListWidgetItem* item)
{
  props_item_c* pi = dynamic_cast <props_item_c*> (item);
  if (!pi)
    {
      return;
    }
  m_ui->soPath->setText (pi->so_path ());
  provider::abstract_provider_c* ap = pi->abstract_provider ();
  int major;
  int minor;
  ap->version (major, minor);
  QString major_s;
  major_s.setNum (major);
  QString minor_s;
  minor_s.setNum (minor);
  QString ver = major_s + QString (".") + minor_s;
  
  QString descr = ap->description ().c_str ();
  m_ui->versionInfo->setText (ver);
  m_ui->descr->setText (descr);
  m_last_selected = pi->prov_id ();
}
// ------------------------------------------------------------------------
void providers_properties_dlg_c::_on_prov_item (QListWidgetItem* item)
{
  if (!item)
    {
      return;
    }
  _display_item (item);
}
// ------------------------------------------------------------------------
void providers_properties_dlg_c::_on_prov_selected (QListWidgetItem* item)
{
  if (!item)
    {
      return;
    }
  _display_item (item);
  props_item_c* pi = dynamic_cast <props_item_c*> (item);
  emit provider_selected (pi->prov_id ());
  if (m_dbl_click_accept)
    {
      accept ();
    }
}
// --------------------------------------------------------------
size_t providers_properties_dlg_c::last_selected () const
{
  return m_last_selected;
}
