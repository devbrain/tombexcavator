#include "abl/platform/mutex/rwlock.h"
#include "thirdparty/predef/predef.h"

#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#include <assert.h>
#else
#include <stdlib.h>
#include <pthread.h>
#endif

struct _rwlock_s
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
static te_bool_t add_writer (rwlock_s* rwlock)
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

static te_bool_t remove_writer (rwlock_s* rwlock)
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
te_bool_t rwlock_read_try_lock (rwlock_s* rwlock)
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE h [2];
#else
  int rc;
#endif
  if (rwlock == NULL)
    {
      return TE_FALSE;
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
      return TE_TRUE;
    case WAIT_TIMEOUT:
      return TE_FALSE;
    } 
  return TE_FALSE;
#else
  rc = pthread_rwlock_tryrdlock(&rwlock->m_mutex);
  return (rc == 0) ? TE_TRUE : TE_FALSE;
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
te_bool_t rwlock_write_try_lock (rwlock_s* rwlock)
{
#if defined(PREDEF_PLATFORM_WIN32)
  HANDLE h [2]; 
#else
  int rc;
#endif
  if (rwlock == NULL)
    {
      return TE_FALSE;
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
      return TE_TRUE;
    case WAIT_TIMEOUT:
      remove_writer (rwlock);
      return TE_FALSE;
    default:
      remove_writer (rwlock);
    } 
  return TE_FALSE;
#else
  rc = pthread_rwlock_trywrlock (&rwlock->m_mutex);
  return (rc == 0) ? TE_TRUE : TE_FALSE;
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
