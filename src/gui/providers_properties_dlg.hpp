#ifndef __PROVIDERS_PROPERTIES_DLG_HPP__
#define __PROVIDERS_PROPERTIES_DLG_HPP__

#include <QtGui/QDialog>
namespace Ui 
{
  class ProvidersPropsDlg;
} // namespace Ui

class QListWidgetItem;

class providers_properties_dlg_c : public QDialog
{
  Q_OBJECT
public:
  providers_properties_dlg_c (QWidget* parent = 0, 
			      bool dbl_click_accept = false);
  void set_caption (const QString& caption);

  size_t last_selected () const;
signals:
  void provider_selected (size_t provider_id);
private:
  void _display_item (QListWidgetItem* item);
private:
  Ui::ProvidersPropsDlg* m_ui;
  bool                   m_dbl_click_accept;
  size_t                 m_last_selected;
private slots:
  void _on_prov_item (QListWidgetItem* item);
  void _on_prov_selected (QListWidgetItem* item);
};


#endif
