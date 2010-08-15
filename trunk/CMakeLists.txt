cmake_minimum_required(VERSION 2.6)

project (tombexcavator)
set (CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# -----------------------------------------------------
# Configure
# -----------------------------------------------------

include (revision_info)
set (SDL_BUILDING_LIBRARY TRUE)
include (FindSDL)
include (FindGlib)
include (TestBigEndian)
include (CheckIncludeFiles)


check_include_files (stdint.h  ciHAVE_STDINT_H)
check_include_files (pthread.h ciHAVE_PTHREAD_H)

if (ciHAVE_STDINT_H)
  set (HAVE_STDINT_H 1)
else ()
  set (HAVE_STDINT_H 0)
endif ()

if (ciHAVE_PTHREAD_H)
  set (HAVE_PTHREAD_H 1)
else ()
  set (HAVE_PTHREAD_H 0)
endif ()

TEST_BIG_ENDIAN (HAVE_BIG_ENDIAN)

if (SDL_FOUND)
  set (HAVE_SDL 1)
else ()
  set (HAVE_SDL 0)
endif ()

if (Glib_FOUND)
  set (HAVE_GLIB 1)
else ()
  set (HAVE_GLIB 0)
endif ()

# -----------------------------------------------------
option (optHAS_OPTIMIZED "Turn Optimizations ON" OFF)
option (optHAS_SYMBOLS   "Build with debug Symbols" ON)

if (optHAS_OPTIMIZED)
  if (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE relwithdebinfo)
  else (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE release)
  endif (optHAS_SYMBOLS)
else (optHAS_OPTIMIZED)
  if (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE Debug)
  else (optHAS_SYMBOLS)
    set (CMAKE_BUILD_TYPE Debug)
  endif (optHAS_SYMBOLS)
endif (optHAS_OPTIMIZED)

# ----------------------------------------------------------
# Compiler Settings
# ----------------------------------------------------------
include (compiler) # Should be last include


# --------------------------------------------------

set (TE_SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR}/src)

set (myLIB_OUTPUT "${CMAKE_BINARY_DIR}/lib/" )
set (myBIN_OUTPUT "${CMAKE_BINARY_DIR}/bin/")
set (myOBJ_OUTPUT "${CMAKE_BINARY_DIR}/obj/")

set (CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${myLIB_OUTPUT})
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${myLIB_OUTPUT})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${myBIN_OUTPUT})

# --------------------------------------------------
# Libs/includes path
# --------------------------------------------------

set (TE_LIBS)
set (TE_INCLUDES)

if (HAVE_SDL)
  list (APPEND TE_LIBS ${SDL_LIBRARY})
  list (APPEND TE_INCLUDES ${SDL_INCLUDE_DIR})
endif ()

if (HAVE_GLIB)
  list (APPEND TE_LIBS     ${Glib_LIBRARIES})
  list (APPEND TE_INCLUDES ${Glib_INCLUDE_DIRS})
endif ()


# --------------------------------------------------
message (STATUS "                   ")
message (STATUS "----- Summary -----")
message (STATUS "                   ")
message (STATUS "Build Type:       ${CMAKE_BUILD_TYPE}")
message (STATUS "BIG_ENDIAN:       ${HAVE_BIG_ENDIAN}")
message (STATUS "stdint.h present: ${HAVE_STDINT_H}")
message (STATUS "pthread.h present: ${HAVE_PTHREAD_H}")
message (STATUS "SDL present:      ${HAVE_SDL}")
message (STATUS "GLIB present:     ${HAVE_GLIB}")
message (STATUS "                  ")
message (STATUS "---- Compiler ----")
message (STATUS "                  ")
message (STATUS "C compiler  : ${CMAKE_C_COMPILER}")
message (STATUS "C++ compiler: ${CMAKE_CXX_COMPILER}")
message (STATUS "C compiler flags: ${myC_FLAGS}")
message (STATUS "C++ compiler flags: ${myCXX_FLAGS}")
message (STATUS "Defines: ${myDEFS}")
message (STATUS "Includes: ${TE_INCLUDES}")
message (STATUS "                  ")
message (STATUS "----- Linker -----")
message (STATUS "                  ")
message (STATUS "Libs: ${TE_LIBS}")
message (STATUS "                  ")
message (STATUS "                  ")
# ==========================================================================
# Create config files
# ==========================================================================

set (TE_CONFIG_DIR ${CMAKE_CURRENT_SOURCE_DIR}/config)
configure_file(${TE_CONFIG_DIR}/te_config.h.in ${TE_CONFIG_DIR}/te_config.h)

# ==========================================================================
include (build)

set (build_modules 
  platform
  archive
)

foreach (mdl ${build_modules})
  set (MDL_SRC "${TE_SRC_ROOT}/${mdl}")
  set (MDL_OBJ "${myOBJ_OUTPUT}/${mdl}")
  add_subdirectory (${MDL_SRC} ${MDL_OBJ})
endforeach ()

include (package)