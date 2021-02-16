cmake_minimum_required (VERSION 3.11)

project(tombexcavator VERSION 0.0.1 LANGUAGES CXX C)

set (CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR};${CMAKE_SOURCE_DIR}/cmake")

# ===========================================================
include(GenerateExportHeader)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads)
find_package(Filesystem)
# ===========================================================


# Output binary to predictable location.
set(BINARY_OUT_DIR ${CMAKE_BINARY_DIR}/bin)
set(LIB_OUT_DIR ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${LIB_OUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${LIB_OUT_DIR})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${BINARY_OUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${BINARY_OUT_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${LIB_OUT_DIR})
# ===========================================================
set (PROJECT_ROOT ${CMAKE_SOURCE_DIR})
set (PROJECT_INCLUDE_PRIVATE ${CMAKE_SOURCE_DIR}/src)
set (PROJECT_INCLUDE         ${CMAKE_SOURCE_DIR}/include)
# ===========================================================
add_subdirectory(ext)
add_subdirectory(src/formats)
add_subdirectory(src/utils)
add_subdirectory(src/unittest)


