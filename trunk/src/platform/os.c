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
/**
 * SDL API
 */

#if defined(TE_USE_SDL)
#include <SDL.h>
#include <assert.h>

note ("Using SDL backend")

#include "platform/os/dl/dl_sdl.c"
#include "platform/os/mutex/mutex_sdl.c"

#endif

#if defined(TE_USE_GLIB)
#include <glib.h>
#include <stdlib.h>
#include <assert.h>

note ("Using GLIB backend")

#include "platform/os/mutex/mutex_glib.c"

#endif

/**
 * NATIVE API
 */

#if defined (TE_USE_NATIVE)

#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#include <assert.h>

note ("Using WIN32 backend")

#include "platform/os/dl/dl_win32.c"
#include "platform/os/mutex/mutex_win32.c"

#elif defined(TE_HAVE_PTHREAD_H)
#define __USE_XOPEN2K
#include <stdlib.h>
#include <pthread.h>
#include <assert.h>

note ("Using PTHREAD backend")

#include "platform/os/mutex/mutex_pthread.c"

#else 
#error "Implement me"
#endif

#if defined(PREDEF_PLATFORM_UNIX)
#include <dlfcn.h>
#include <stdlib.h>
#include <assert.h>
note ("Using UNIX backend")
#include "platform/os/dl/dl_unix.c"
#else
#error "Implement me"
#endif

#endif

