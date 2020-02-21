include(ExternalProject)
if (NOT VFS_INSTALL_ROOT)
	if (NOT IS_SINGLE_CONFIGURATION)
		set(propagate_build_type --config $<CONFIG> -DCMAKE_BUILD_TYPE=$<CONFIG>)
	else()
		set(propagate_build_type -DCMAKE_BUILD_TYPE=$<CONFIG>)
	endif()

	set (VFS_INSTALL_ROOT ${CMAKE_BINARY_DIR})
	ExternalProject_Add(vfs_ext
			GIT_REPOSITORY git@github.com:devbrain/vfs.git
			INSTALL_DIR    ${VFS_INSTALL_ROOT}
			CMAKE_ARGS
				-DCMAKE_INSTALL_PREFIX:PATH=<INSTALL_DIR>
				-DCMAKE_PREFIX_PATH=${CMAKE_CURRENT_BINARY_DIR}
				-DBUILD_TEST:BOOL=OFF
				-DBSW_INSTALL_ROOT=${CMAKE_BINARY_DIR}
				${propagate_build_type}
			UPDATE_COMMAND    ""
			BUILD_ALWAYS      OFF
		)
endif()
	    
add_library(vfs::vfs SHARED IMPORTED GLOBAL)

if (WINDOWS)
  set (BSW_LIB vfs_api.dll)
else ()
  set (BSW_LIB libvfs_api.so)
endif()

#make cmake happy about INTERFACE_INCLUDE_DIRECTORIES
file (MAKE_DIRECTORY ${VFS_INSTALL_ROOT}/include)

if (NOT IS_SINGLE_CONFIGURATION)
set_target_properties(
  vfs::vfs PROPERTIES
  IMPORTED_LOCATION_DEBUG   ${VFS_INSTALL_ROOT}/lib/Debug/${BSW_LIB}
  IMPORTED_LOCATION_RELEASE ${VFS_INSTALL_ROOT}/lib/Release/${BSW_LIB}
  IMPORTED_IMPLIB_DEBUG     ${VFS_INSTALL_ROOT}/lib/Debug/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  IMPORTED_IMPLIB_RELEASE   ${VFS_INSTALL_ROOT}/lib/Release/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  INTERFACE_COMPILE_DEFINITIONS ""
  INTERFACE_INCLUDE_DIRECTORIES ${VFS_INSTALL_ROOT}/include
  )
else ()
  set_target_properties(
  vfs::vfs PROPERTIES
  IMPORTED_LOCATION   ${VFS_INSTALL_ROOT}/lib/${CMAKE_BUILD_TYPE}/${BSW_LIB}
  IMPORTED_IMPLIB     ${VFS_INSTALL_ROOT}/lib/${CMAKE_BUILD_TYPE}/bsw${CMAKE_IMPORT_LIBRARY_SUFFIX}
  INTERFACE_INCLUDE_DIRECTORIES ${VFS_INSTALL_ROOT}/include
  INTERFACE_COMPILE_DEFINITIONS "" 	  
  )
endif()
# ==============================================================================
set_target_properties(vfs_ext PROPERTIES FOLDER ${DEPENDENCY_FOLDER})
add_dependencies(vfs_ext bsw_ext)
