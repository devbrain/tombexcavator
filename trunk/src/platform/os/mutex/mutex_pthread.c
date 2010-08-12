struct mutex_s_
{
  pthread_mutex_t m_mutex;
};
/* ------------------------------------------------- */
mutex_s* mutex_create (void)
{
  int failed = 0;
  mutex_s* m = 0;
  
  m = (mutex_s*) malloc (sizeof (mutex_s));
  failed = (pthread_mutex_init (&m->m_mutex, NULL) != 0);
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
  pthread_mutex_lock (&mutex->m_mutex);
}
/* ------------------------------------------------- */
void mutex_unlock (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
  pthread_mutex_unlock (&mutex->m_mutex);
}
/* ------------------------------------------------- */
void mutex_destroy (mutex_s* mutex)
{
  if (!mutex)
    {
      return;
    }
  pthread_mutex_destroy (&mutex->m_mutex);
  free (mutex);
}
