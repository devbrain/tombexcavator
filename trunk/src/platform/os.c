#include "platform/os.h"
#include "thirdparty/predef/predef.h"



#if defined(TE_HAVE_SDL)
#define TE_USE_SDL
#elif defined (TE_HAVE_GLIB)
#define TE_USE_GLIB
#include <glib.h>
#else
#define TE_USE_NATIVE
#endif
/**
 * SDL API
 */
#if defined(TE_USE_SDL)
#include <SDL.h>
#include <assert.h>

/**
 * SHARED OBJECTS API
 */
struct shared_object_s_
{
  void* handle;
};

shared_object_s* shared_object_open (const char* path)
{
  shared_object_s* result;
  void* h = SDL_LoadObject (path);

  if (h == NULL) 
    {
      return 0;
    }
  result = malloc (sizeof (shared_object_s));
  assert (result != NULL);
  result->handle = h;
  return result;
}
/* ------------------------------------------------------ */
void* shared_object_get_symbol (shared_object_s* so, 
				const char* name)
{
  if (so == NULL || so->handle == NULL)
    {
      return NULL;
    }
  return SDL_LoadFunction (so->handle, name); 
}

void shared_object_close (shared_object_s* so)
{
  if (so == NULL)
    {
      return;
    }
  SDL_UnloadObject (so->handle);
  free (so);
}

#endif


/**
 * NATIVE API
 */

#if defined (TE_USE_NATIVE)

#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#elif defined(PREDEF_PLATFORM_UNIX)
#define __USE_XOPEN2K
#include <sys/time.h>
#include <sys/types.h>
#include <dlfcn.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#else 
#error "Implement me"
#endif

#include <assert.h>

/**
 * SHARED OBJECTS API
 */
#if defined(PREDEF_PLATFORM_WIN32)
typedef HMODULE shared_object_handle_t;
#else
typedef void*  shared_object_handle_t;
#endif

struct shared_object_s_
{
  shared_object_handle_t handle;
};

/* --------------------------------------------------------- */

shared_object_s* shared_object_open (const char* path)
{
  shared_object_s* result = 0;
  shared_object_handle_t         h;
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
#elif defined(PREDEF_PLATFORM_UNIX)
  h = dlopen (path, RTLD_LAZY);
  if (!h)
    {
      return 0;
    }
#else
  #error "Implement me"
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
#elif defined(PREDEF_PLATFORM_UNIX)
  return dlsym (so->handle, name);
#else
#error "Implement Me"
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
#elif defined(PREDEF_PLATFORM_UNIX)
  dlclose (so->handle);
#else
#error "Implement Me"
#endif
  free (so);
}

/**
 * MUTEX API
 */

struct mutex_s_
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE m_mutex;
#elif defined(PREDEF_PLATFORM_UNIX)
  pthread_mutex_t m_mutex;
#else
  #error "Implement me"
#endif
};

mutex_s* mutex_create (void)
{
  int failed = 0;
  mutex_s* m = 0;
  
  m = (mutex_s*) malloc (sizeof (mutex_s));
#if defined(PREDEF_PLATFORM_WIN32)
  m->m_mutex = CreateMutex (NULL, FALSE, NULL);
  if (m->m_mutex = NULL)
    {
      failed = 1;
    }
#elif defined (PREDEF_PLATFORM_UNIX)
  failed = (pthread_mutex_init (&m->m_mutex, NULL) != 0);
#else
#error "Implement me"
#endif
  if (failed)
    {
      free (m);
      m = 0;
    }
  return m;
}

/* ------------------------------------------------- */

int mutex_try_lock (mutex_s* mutex)
{
  int rc;
  if (!mutex)
    {
      return 0;
    }
#if !defined(PREDEF_PLATFORM_WIN32)
  rc = pthread_mutex_trylock (&mutex->m_mutex);
  if (rc == 0)
    return 1;
  return 0; 
#elif defined (PREDEF_PLATFORM_UNIX)
  rc = WaitForSingleObject(mutex->m_mutex, 0);
  return (rc == WAIT_OBJECT_0);
#else
#error "Implement me"
#endif
}

/* ------------------------------------------------- */

int mutex_try_lock_timeout (mutex_s* mutex, int msecs)
{
  int rc;
  
#if defined(PREDEF_PLATFORM_UNIX)
  struct timespec abstime;
  struct timeval tv;

  if (!mutex)
    {
      return 0;
    }

  gettimeofday(&tv, NULL);
  abstime.tv_sec  = tv.tv_sec + msecs / 1000;
  abstime.tv_nsec = tv.tv_usec*1000 + (msecs % 1000)*1000000;
  if (abstime.tv_nsec >= 1000000000)
    {
      abstime.tv_nsec -= 1000000000;
      abstime.tv_sec++;
    }
  rc = pthread_mutex_timedlock(&mutex->m_mutex, &abstime);
  if (rc == 0)
    return 1;
  
  return 0;
#elif defined (PREDEF_PLATFORM_WIN32)
  if (!mutex)
    {
      return 0;
    }
  rc = WaitForSingleObject(mutex->m_mutex, msecs);
  return (rc == WAIT_OBJECT_0);
#else
#error "Implement me"
#endif
}

void mutex_lock (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  WaitForSingleObject(mutex->m_mutex, INFINITE);
#elif defined (PREDEF_PLATFORM_UNIX)
  pthread_mutex_lock (&mutex->m_mutex);
#else
#error "Implement me"
#endif
}

/* ------------------------------------------------- */

void mutex_unlock (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  if (mutex->m_mutex == NULL)
    {
      return;
    }
  ReleaseMutex (mutex->m_mutex);
#elif defined (PREDEF_PLATFORM_UNIX)
  pthread_mutex_unlock (&mutex->m_mutex);
#else
#error "Implement me"
#endif
}

/* ------------------------------------------------- */

void mutex_destroy (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  if (mutex->m_mutex == NULL)
    {
      return;
    }
  CloseHandle (mutex->m_mutex);
#elif defined (PREDEF_PLATFORM_UNIX)
  pthread_mutex_destroy (&mutex->m_mutex);
#else
#error "Implemenet me"
#endif
  free (mutex);
}

struct rwlock_s_
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE   m_mutex;
  HANDLE   m_read_event;
  HANDLE   m_write_event;
  unsigned m_readers;
  unsigned m_writers_waiting;
  unsigned m_writers; 
#else
  pthread_rwlock_t m_mutex;
#endif
};

/* -------------------------------------------------------- */
#if defined(PREDEF_PLATFORM_WIN32)
static int add_writer (rwlock_s* rwlock)
{
  if (WaitForSingleObject (rwlock->m_mutex, INFINITE) == WAIT_OBJECT_0)
    {
      if (++rwlock->m_writers_waiting == 1) 
	{
	  ResetEvent (rwlock->m_read_event);
	}
      ReleaseMutex(rwlock->m_mutex);
      return 1;
    } 
  return 0;
}

static int remove_writer (rwlock_s* rwlock)
{
  if (WaitForSingleObject (rwlock->m_mutex, INFINITE) == WAIT_OBJECT_0)
    {
      if (--rwlock->m_writers_waiting == 0 && rwlock->m_writers == 0) 
	{
	  SetEvent(rwlock->m_read_event);
	}
      ReleaseMutex(rwlock->m_mutex);
      return 1;
    } 
  return 0;
}

#endif
/* -------------------------------------------------------- */
rwlock_s* rwlock_create (void)
{
  rwlock_s* result = NULL;
  result = (rwlock_s*) malloc (sizeof (rwlock_s));
#if defined(PREDEF_PLATFORM_WIN32)
  result->m_readers         = 0;
  result->m_writers         = 0;
  result->m_writers_waiting = 0;
  result->m_mutex = CreateMutexW (NULL, FALSE, NULL);
  if (result->m_mutex == NULL)
    {
      goto rwlock_error;
    }
  result->m_read_event = CreateEventW(NULL, TRUE, TRUE, NULL); 
  if (result->m_read_event == NULL)
    {
      goto rwlock_error;
    }
  result->m_write_event = CreateEventW(NULL, TRUE, TRUE, NULL); 
  if (result->m_write_event == NULL)
    {
      goto rwlock_error;
    }
#else
  if (pthread_rwlock_init (&result->m_mutex, NULL))
    {
      goto rwlock_error;
    }
#endif
  goto rwlock_ok;

 rwlock_error:;
  free (result);
  result = NULL;
 rwlock_ok:;
  return result;
}
/* -------------------------------------------------------- */
void rwlock_read_lock (rwlock_s* rwlock)
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE h [2];
#endif
  if (rwlock == NULL)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  h [0] = rwlock->m_mutex;
  h [1] = rwlock->m_read_event;
  switch (WaitForMultipleObjects (2, h, TRUE, INFINITE))
    {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0 + 1:
      ++rwlock->m_readers;
      ResetEvent (rwlock->m_write_event);
      ReleaseMutex (rwlock->m_mutex);
      assert (rwlock->m_writers == 0);
      break;
    } 
#else
  pthread_rwlock_rdlock (&rwlock->m_mutex);
#endif
}
/* -------------------------------------------------------- */
int rwlock_read_try_lock (rwlock_s* rwlock)
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE h [2];
#else
  int rc;
#endif
  if (rwlock == NULL)
    {
      return 0;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  h [0] = rwlock->m_mutex;
  h [1] = rwlock->m_read_event;
  switch (WaitForMultipleObjects (2, h, TRUE, 1))
    {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0 + 1:
      ++rwlock->m_readers;
      ResetEvent (rwlock->m_write_event);
      ReleaseMutex (rwlock->m_mutex);
      assert (rwlock->m_writers == 0);
      return 1;
    case WAIT_TIMEOUT:
      return 0;
    } 
  return 0;
#else
  rc = pthread_rwlock_tryrdlock(&rwlock->m_mutex);
  return (rc == 0) ? 1 : 0;
#endif
}
/* -------------------------------------------------------- */
void rwlock_write_lock (rwlock_s* rwlock)
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE h [2]; 
#endif
  if (rwlock == NULL)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  add_writer (rwlock);
  h[0] = rwlock->m_mutex;
  h[1] = rwlock->m_write_event;
  switch (WaitForMultipleObjects (2, h, TRUE, INFINITE))
    {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0 + 1:
      --rwlock->m_writers_waiting;
      ++rwlock->m_readers;
      ++rwlock->m_writers;
      ResetEvent (rwlock->m_read_event);
      ResetEvent (rwlock->m_write_event);
      ReleaseMutex (rwlock->m_mutex);
      assert (rwlock->m_writers == 1);
      break;
    default:
      remove_writer (rwlock);
    } 
#else
  pthread_rwlock_wrlock (&rwlock->m_mutex);
#endif
}
/* -------------------------------------------------------- */
int rwlock_write_try_lock (rwlock_s* rwlock)
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE h [2]; 
#else
  int rc;
#endif
  if (rwlock == NULL)
    {
      return 0;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  add_writer (rwlock);
  h[0] = rwlock->m_mutex;
  h[1] = rwlock->m_write_event;
  switch (WaitForMultipleObjects (2, h, TRUE, 1))
    {
    case WAIT_OBJECT_0:
    case WAIT_OBJECT_0 + 1:
      --rwlock->m_writers_waiting;
      ++rwlock->m_readers;
      ++rwlock->m_writers;
      ResetEvent (rwlock->m_read_event);
      ResetEvent (rwlock->m_write_event);
      ReleaseMutex (rwlock->m_mutex);
      assert (rwlock->m_writers == 1);
      return 1;
    case WAIT_TIMEOUT:
      remove_writer (rwlock);
      return 0;
    default:
      remove_writer (rwlock);
    } 
  return 0;
#else
  rc = pthread_rwlock_trywrlock (&rwlock->m_mutex);
  return (rc == 0) ? 1 : 0;
#endif
}
/* -------------------------------------------------------- */
void rwlock_unlock (rwlock_s* rwlock)
{
  if (rwlock == NULL)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  if (WaitForSingleObject(rwlock->m_mutex, INFINITE) == WAIT_OBJECT_0)
    {
      rwlock->m_writers = 0;
      if (rwlock->m_writers_waiting == 0) 
	{
	  SetEvent (rwlock->m_read_event);
	}
      if (--rwlock->m_readers == 0) 
	{
	  SetEvent (rwlock->m_write_event);
	}
      ReleaseMutex(rwlock->m_mutex); 
    }
#else
  pthread_rwlock_unlock (&rwlock->m_mutex);
#endif
}
/* -------------------------------------------------------- */
void rwlock_destroy (rwlock_s* rwlock)
{
  if (rwlock == NULL)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  CloseHandle(rwlock->m_mutex);
  CloseHandle(rwlock->m_read_event);
  CloseHandle(rwlock->m_write_event); 
#else
  pthread_rwlock_destroy(&rwlock->m_mutex); 
#endif
  free (rwlock);
}
#endif 

/* END OF NATIVE API */
