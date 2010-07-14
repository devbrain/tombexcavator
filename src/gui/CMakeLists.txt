# QT4 Handling
FIND_PACKAGE (Qt4 REQUIRED)
 
INCLUDE (${QT_USE_FILE})
 
SET (GUI_QT_SRC 
  specify_providers_dlg.hpp
  main_window.hpp
  providers_properties_dlg.hpp
  vfs_browser.hpp
  models/vfs_model.hpp
)

SET (GUI_QT_UI 
  ui/specify_providers.ui
  ui/providers_loader.ui
  ui/main_widget.ui
  ui/providers_properties.ui
)

SET (GUI_QT_RES)
 
QT4_WRAP_CPP (GUI_MOC_CPP ${GUI_QT_SRC})
QT4_WRAP_UI  (GUI_UI_CPP ${GUI_QT_UI})
QT4_ADD_RESOURCES (GUI_RES_H ${GUI_QT_RES})
 
# General
SET (GUI_SRC
  main.cpp
  settings.cpp
  specify_providers_dlg.cpp
  providers_loader_dlg.cpp
  main_window.cpp
  providers_registry.cpp
  providers_properties_dlg.cpp
  vfs_browser.cpp

  models/vfs_model.cpp

  ${GUI_MOC_CPP}
  ${GUI_UI_CPP}
  ${GUI_RES_H}
)

SET (GUI_HDR
  settings.hpp
  specify_providers_dlg.hpp
  providers_loader_dlg.hpp
  main_window.hpp
  providers_registry.hpp
  vfs_browser.hpp
  models/vfs_model.hpp
)
 
SET (GUI_LIB
  provider
  vfs
  abl
  ${QT_LIBRARIES}
)

INCLUDE_DIRECTORIES ()
 
ADD_EXECUTABLE        (tomb_excavator_gui ${GUI_SRC} )
TARGET_LINK_LIBRARIES (tomb_excavator_gui ${GUI_LIB} )
