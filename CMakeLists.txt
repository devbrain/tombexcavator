cmake_minimum_required (VERSION 3.11)

project(tombexcavator VERSION 0.0.1 LANGUAGES CXX C)


set (CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_SOURCE_DIR}/cmake-scripts")


include("onyx")

onyx_setup_conan(
  REQUIRES
  vfs/0.0.1@devbrain/stable
  bsw/0.0.1@devbrain/stable
  doctest/2.3.4@bincrafters/stable
  OPTIONS
  bsw:shared=True
  vfs:shared=True
)

# ===========================================================
include(GenerateExportHeader)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)

# ===========================================================


# Output binary to predictable location.
set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_OUT_DIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${LIB_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUT_DIR})


# ===========================================================
set (PROJECT_ROOT ${CMAKE_SOURCE_DIR})
# ===========================================================

find_package(bsw REQUIRED)
find_package(doctest REQUIRED)
find_package(vfs REQUIRED)

# ===========================================================
add_subdirectory(ext)
add_subdirectory(src/shell)
add_subdirectory(src/unittest)
add_subdirectory(src/archive)


