#include <stdlib.h>

#include "abl/platform/mutex/mutex.h"
#include "thirdparty/predef/predef.h"

#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
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

te_bool_t mutex_try_lock (mutex_s* mutex)
{
  int rc;
  if (!mutex)
    {
      return 0;
    }
#if !defined(PREDEF_PLATFORM_WIN32)
  rc = pthread_mutex_trylock (&mutex->m_mutex);
  if (rc == 0)
    return TE_TRUE;
  return TE_FALSE; 
#else
  rc = WaitForSingleObject(mutex->m_mutex, 0);
  return (rc == WAIT_OBJECT_0);
#endif
}

/* ------------------------------------------------- */

te_bool_t mutex_try_lock_timeout (mutex_s* mutex, int msecs)
{
  int rc;
  if (!mutex)
    {
      return 0;
    }
#if !defined(PREDEF_PLATFORM_WIN32)
  struct timespec abstime;
  struct timeval tv;
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
    return TE_TRUE;
  
  return TE_FALSE;
#else
  rc = WaitForSingleObject(mutex->m_mutex, msecs);
  return (rc == WAIT_OBJECT_0);
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

