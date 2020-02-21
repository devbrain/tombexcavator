include(ExternalProject)


set (MIO_INSTALL_ROOT ${CMAKE_BINARY_DIR}/include/mio)
ExternalProject_Add(mio_ext
	GIT_REPOSITORY git@github.com:mandreyel/mio.git
	INSTALL_DIR    ${MIO_INSTALL_ROOT}
	UPDATE_COMMAND    ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/single_include/mio/mio.hpp <INSTALL_DIR>/mio.hpp
	CONFIGURE_COMMAND ""
	BUILD_ALWAYS      OFF
)

add_library(mio::mio INTERFACE IMPORTED GLOBAL)


#make cmake happy about INTERFACE_INCLUDE_DIRECTORIES
file (MAKE_DIRECTORY ${MIO_INSTALL_ROOT})

if (NOT IS_SINGLE_CONFIGURATION)
set_target_properties(
  mio::mio PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS ""
  INTERFACE_INCLUDE_DIRECTORIES ${MIO_INSTALL_ROOT}
  )
else ()
  set_target_properties(
  mio::mio PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${MIO_INSTALL_ROOT}
  INTERFACE_COMPILE_DEFINITIONS "" 	  
  )
endif()
# ==============================================================================
set_target_properties(mio_ext PROPERTIES FOLDER ${DEPENDENCY_FOLDER})

