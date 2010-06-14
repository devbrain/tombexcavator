#ifndef __PROVIDERS_LOADER_DLG_HPP__
#define __PROVIDERS_LOADER_DLG_HPP__

#include <QtGui/QDialog>

namespace Ui 
{
  class providers_loader;
} // namespace Ui

class QWidget;

class providers_loader_dlg_c : public QDialog
{
public:
  providers_loader_dlg_c (QWidget* parent = 0);
private:
  Ui::providers_loader* m_ui;
};


#endif
