find_package(Qt5 COMPONENTS Core)
if (NOT Qt5_FOUND)
    message("QT5 is missing")
else()
    message("Found QT5")
    SET(BUILD_TE_UI TRUE)
endif()

add_subdirectory(cli)
