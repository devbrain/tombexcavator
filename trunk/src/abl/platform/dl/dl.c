#include <errno.h>
#include <stdlib.h>

#include "abl/platform/dl/dl.h"

#include "thirdparty/predef/predef.h"

#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#else
#include <dlfcn.h>
#endif

#if defined(PREDEF_PLATFORM_WIN32)
typedef HMODULE handle_t;
#else
typedef void*  handle_t;
#endif

/* --------------------------------------------------------- */

struct _shared_object_s
{
  handle_t handle;
};

/* --------------------------------------------------------- */

shared_object_s* shared_object_open (const char* path)
{
  shared_object_s* result = 0;
  handle_t         h;
  if (!path)
    {
      return 0;
    }
  
#if defined(PREDEF_PLATFORM_WIN32)
  h = LoadLibrary (path);
  if (!h)
    {
      return 0;
    }
#else
  h = dlopen (path, RTLD_LAZY);
  if (!h)
    {
      return 0;
    }
#endif
  result = (shared_object_s*)malloc (sizeof (shared_object_s));
  result->handle = h;
  return result;
}
/* --------------------------------------------------------- */
void* shared_object_get_symbol (shared_object_s* so, const char* name)
{
  if (!so || !so->handle)
    {
      return 0;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  return GetProcAddress (so->handle, name);
#else
  return dlsym (so->handle, name);
#endif
}
/* --------------------------------------------------------- */
void shared_object_close (shared_object_s* so)
{
  if (!so || !so->handle)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  FreeLibrary (so->handle);
#else
  dlclose (so->handle);
#endif
}
