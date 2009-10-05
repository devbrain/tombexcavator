#include <stdlib.h>

#include "abl/platform/mutex/mutex.h"
#include "thirdparty/predef/predef.h"

#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#else
#include <pthread.h>
#endif


struct _mutex_s
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE m_mutex;
#else
  pthread_mutex_t m_mutex;
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
#else
  failed = (pthread_mutex_init (&m->m_mutex, NULL) != 0);
#endif
  if (failed)
    {
      free (m);
      m = 0;
    }
  return m;
}

/* ------------------------------------------------- */

void mutex_lock (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  WaitForSingleObject(mutex->m_mutex, INFINITE);
#else
  pthread_mutex_lock (&mutex->m_mutex);
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
#else
  pthread_mutex_unlock (&mutex->m_mutex);
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
#else
  pthread_mutex_destroy (&mutex->m_mutex);
#endif
}
