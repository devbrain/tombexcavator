#define _POSIX_C_SOURCE  200112L

#include "platform/os.h"
#include "platform/config.h"
#include "platform/fixme.h"

#include "thirdparty/predef/predef.h"

#if defined(TE_HAVE_SDL)
#define TE_USE_SDL
#elif defined (TE_HAVE_GLIB)
#define TE_USE_GLIB
#else
#define TE_USE_NATIVE
#endif

/* ==================================== 
 * Deduce includes
 * ==================================== */

#if defined(TE_USE_SDL)
#   define DO_INCLUDE_SDL
#   define DO_INCLUDE_NATIVE_FS
#elif defined(TE_USE_GLIB)
#   define DO_INCLUDE_GLIB
#   define DO_INCLUDE_NATIVE_FS
#   define DO_INCLUDE_NATIVE_DL
#elif defined(TE_USE_NATIVE)
#   define DO_INCLUDE_NATIVE_FS
#   define DO_INCLUDE_NATIVE_DL
#   if defined(TE_HAVE_PTHREAD_H)
#      define DO_INCLUDE_PTHREAD
#   endif
#else
#  error "Implement me"
#endif

/* native includes */

#if defined(PREDEF_PLATFORM_WIN32)
# if defined(DO_INCLUDE_NATIVE_FS) || defined(DO_INCLUDE_NATIVE_DL)
#    include <windows.h>
# endif
#elif defined(PREDEF_PLATFORM_UNIX)
#  if defined(DO_INCLUDE_NATIVE_FS)
#     include <sys/types.h>
#     include <sys/stat.h>
#     include <fcntl.h>
#     include <unistd.h>
#     include <dirent.h>
#     include <errno.h>
#  endif
#  if defined(DO_INCLUDE_NATIVE_DL)
#     include <dlfcn.h>
#  endif
#else
#error "Implement me"
#endif

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#if defined(DO_INCLUDE_PTHREAD)
#include <pthread.h>
#endif

#if defined(DO_INCLUDE_SDL)
#include <SDL.h>
#endif

#if defined(DO_INCLUDE_GLIB)
#include <glib.h>
#endif

/**
 * SDL API
 */

#if defined(TE_USE_SDL)
#   include "platform/os/dl/dl_sdl.c"
#   include "platform/os/mutex/mutex_sdl.c"
#endif

#if defined(TE_USE_GLIB)
#include "platform/os/mutex/mutex_glib.c"
#if defined(PREDEF_PLATFORM_WIN32)
#   include "platform/os/dl/dl_win32.c"
#elif defined(PREDEF_PLATFORM_UNIX)
#   include "platform/os/dl/dl_unix.c"
#else
#   error "Implement Me"
#endif

#endif

/**
 * NATIVE API
 */

#if defined (TE_USE_NATIVE)

#if defined(PREDEF_PLATFORM_WIN32)
note ("Using WIN32 backend")
#include "platform/os/dl/dl_win32.c"
#elif defined(PREDEF_PLATFORM_UNIX)
#include "platform/os/dl/dl_unix.c"
#else
#error "Implement me"
#endif

#if defined(PREDEF_PLATFORM_WIN32)
#include "platform/os/mutex/mutex_win32.c"
#elif defined(TE_HAVE_PTHREAD_H)
#include "platform/os/mutex/mutex_pthread.c"
#else 
#error "Implement me"
#endif


#endif
/*
  Files API
 */
#define SET_ERROR(err) if (*err) *err = create_os_error_from_last_error ()

#if defined(PREDEF_PLATFORM_WIN32)
#     include "platform/os/fs/fs_win32.c"
#elif defined(PREDEF_PLATFORM_UNIX)
#     include "platform/os/fs/fs_unix.c"
#else
#   error "Implement me"
#endif


