struct os_mutex_s_
{
  pthread_mutex_t m_mutex;
};
/* ------------------------------------------------- */
os_mutex_s* os_mutex_create (void)
{
  int failed = 0;
  os_mutex_s* m = 0;
  
  m = (os_mutex_s*) malloc (sizeof (os_mutex_s));
  failed = (pthread_mutex_init (&m->m_mutex, NULL) != 0);
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
  pthread_mutex_lock (&mutex->m_mutex);
}
/* ------------------------------------------------- */
void os_mutex_unlock (os_mutex_s* mutex)
{
  assert (mutex != NULL);
  pthread_mutex_unlock (&mutex->m_mutex);
}
/* ------------------------------------------------- */
void os_mutex_destroy (os_mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
  pthread_mutex_destroy (&mutex->m_mutex);
  free (mutex);
}
