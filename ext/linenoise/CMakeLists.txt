include(ExternalProject)


set (LINENOISE_INSTALL_ROOT ${CMAKE_BINARY_DIR}/include/linenoise)
ExternalProject_Add(linenoise_ext
	GIT_REPOSITORY git@github.com:yhirose/cpp-linenoise.git
	INSTALL_DIR    ${LINENOISE_INSTALL_ROOT}
	UPDATE_COMMAND    ""
	BUILD_COMMAND ""
	INSTALL_COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/linenoise.hpp <INSTALL_DIR>/linenoise.hpp
	CONFIGURE_COMMAND ""
	BUILD_ALWAYS      OFF
)

add_library(linenoise::linenoise INTERFACE IMPORTED GLOBAL)


#make cmake happy about INTERFACE_INCLUDE_DIRECTORIES
file (MAKE_DIRECTORY ${LINENOISE_INSTALL_ROOT})

if (NOT IS_SINGLE_CONFIGURATION)
set_target_properties(
  linenoise::linenoise PROPERTIES
  INTERFACE_COMPILE_DEFINITIONS ""
  INTERFACE_INCLUDE_DIRECTORIES ${LINENOISE_INSTALL_ROOT}
  )
else ()
  set_target_properties(
  linenoise::linenoise PROPERTIES
  INTERFACE_INCLUDE_DIRECTORIES ${LINENOISE_INSTALL_ROOT}
  INTERFACE_COMPILE_DEFINITIONS "" 	  
  )
endif()
# ==============================================================================
set_target_properties(linenoise_ext PROPERTIES FOLDER ${DEPENDENCY_FOLDER})

