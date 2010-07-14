#ifndef __GUI_SPECIFY_PROVIDERS_DLG_HPP__
#define __GUI_SPECIFY_PROVIDERS_DLG_HPP__

#include <QtGui/QDialog>

namespace Ui 
{
  class specify_providers;
} // namespace Ui


class QWidget;

class specify_providers_dlg_c : public QDialog
{
  Q_OBJECT
public:
  specify_providers_dlg_c (QWidget* parent);
private:
  virtual void 	accept ();
  virtual void 	reject ();
private slots:
   void _on_browse (); 
private:
  Ui::specify_providers* m_ui;
};


#endif
