struct mutex_s_
{
  HANDLE m_mutex;
};
/* ------------------------------------------------- */
mutex_s* mutex_create (void)
{
  int failed = 0;
  mutex_s* m = 0;
  
  m = (mutex_s*) malloc (sizeof (mutex_s));
  m->m_mutex = CreateMutex (NULL, FALSE, NULL);
  if (m->m_mutex = NULL)
    {
      failed = 1;
    }
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
  WaitForSingleObject(mutex->m_mutex, INFINITE);
}
/* ------------------------------------------------- */
void mutex_unlock (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
  if (mutex->m_mutex == NULL)
    {
      return;
    }
  ReleaseMutex (mutex->m_mutex);
}
/* ------------------------------------------------- */
void mutex_destroy (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
  if (mutex->m_mutex == NULL)
    {
      return;
    }
  CloseHandle (mutex->m_mutex);
  free (mutex);
}
