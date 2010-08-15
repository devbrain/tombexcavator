struct os_mutex_s_
{
  HANDLE m_mutex;
};
/* ------------------------------------------------- */
os_mutex_s* os_mutex_create (void)
{
  int failed = 0;
  os_mutex_s* m = 0;
  
  m = (os_mutex_s*) malloc (sizeof (os_mutex_s));
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
void os_mutex_lock (os_mutex_s* mutex)
{
  assert (mutex != NULL);
  WaitForSingleObject(mutex->m_mutex, INFINITE);
}
/* ------------------------------------------------- */
void os_mutex_unlock (os_mutex_s* mutex)
{
  assert (mutex != NULL);

  if (mutex->m_mutex == NULL)
    {
      return;
    }
  ReleaseMutex (mutex->m_mutex);
}
/* ------------------------------------------------- */
void os_mutex_destroy (os_mutex_s* mutex)
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
