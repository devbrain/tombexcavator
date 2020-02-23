cmake_minimum_required (VERSION 3.11)

project(tombexcavator)

option(BUILD_SHARED_LIBS "Build shared library" ON)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
string(COMPARE EQUAL "${CMAKE_CFG_INTDIR}" "." IS_SINGLE_CONFIGURATION)

if(NOT EXISTS "${CMAKE_SOURCE_DIR}/cmake/conan/conan.cmake")
  if (NOT EXISTS ${CMAKE_BINARY_DIR}/conan.cmake)
        message(STATUS "Downloading conan.cmake from https://github.com/memsharded/cmake-conan")
        file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/master/conan.cmake"
                "${CMAKE_BINARY_DIR}/conan.cmake"
                SHOW_PROGRESS STATUS status)
        message(STATUS "Download status = ${status}")
  else ()
        message (STATUS "Using existing ${CMAKE_BINARY_DIR}/conan.cmake")
  endif ()
  include(${CMAKE_BINARY_DIR}/conan.cmake)
else()
  include(${CMAKE_SOURCE_DIR}/cmake/conan/conan.cmake)
endif ()

conan_cmake_run(CONANFILE conanfile.txt 
  BASIC_SETUP CMAKE_TARGETS
  BUILD missing)

conan_load_buildinfo ()

conan_basic_setup(NO_OUTPUT_DIRS)

# ===========================================================
include(GenerateExportHeader)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
find_package(Filesystem REQUIRED Experimental Final)
# ===========================================================

# Organize unrelated targets to clean IDE hierarchy.
set(DEPENDENCY_FOLDER "Dependencies")

# This will also clean up the CMake ALL_BUILD, INSTALL, RUN_TESTS and ZERO_CHECK projects.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER ${DEPENDENCY_FOLDER})

# Output binary to predictable location.
set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_OUT_DIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUT_DIR})

if (NOT IS_SINGLE_CONFIGURATION)
    foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
	    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIGU)
	    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIGU} ${BINARY_OUT_DIR}/${OUTPUTCONFIG})
	    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIGU} ${LIB_OUT_DIR}/${OUTPUTCONFIG})
	    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIGU} ${LIB_OUT_DIR}/${OUTPUTCONFIG})
    endforeach(OUTPUTCONFIG CMAKE_CONFIGURATION_TYPES)
endif()
# ===========================================================
set (PROJECT_ROOT ${CMAKE_SOURCE_DIR})
# ===========================================================
add_subdirectory(ext)
add_subdirectory(src/shell)


