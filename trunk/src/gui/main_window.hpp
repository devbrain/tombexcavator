#ifndef __MAIN_WINDOW_HPP__
#define __MAIN_WINDOW_HPP__

#include <QtCore/QMap>
#include <QtGui/QMainWindow>

namespace Ui 
{
  class MainWindow;
} // namespace Ui

class providers_properties_dlg_c;

class main_window_c : public QMainWindow
{
  Q_OBJECT
public:
  main_window_c (QWidget* parent = 0);
protected:
  void closeEvent(QCloseEvent *event);
private:
  void _setup_actions ();
  void _add_fs (size_t provider_id);
private:
  typedef QMap <size_t, QWidget*> fs_map_t;
private:
  Ui::MainWindow*             m_ui;
  providers_properties_dlg_c* m_prov_props;
  providers_properties_dlg_c* m_prov_chooser;
  fs_map_t                    m_fs_map;
private slots:
  void _request_exit ();
  void _show_providers_props ();
  void _mount_fs ();
};


#endif
