struct mutex_s_
{
  GMutex* m;
};
/* ---------------------------------------- */
mutex_s* mutex_create (void)
{
  mutex_s* result = NULL;
  GMutex* m = g_mutex_new ();
  if (m == NULL)
    {
      return NULL;
    }
  result = malloc (sizeof (mutex_s));
  result->m = m;
  return result;
}
/* ---------------------------------------- */
void mutex_lock (mutex_s* mutex)
{
  assert (mutex != NULL);
  g_mutex_lock (mutex->m);
}
/* ---------------------------------------- */
void mutex_unlock (mutex_s* mutex)
{
  assert (mutex != NULL);
  
  g_mutex_unlock (mutex->m);
}
/* ---------------------------------------- */
void mutex_destroy (mutex_s* mutex)
{
  if (mutex == NULL)
    {
      return;
    }
  g_mutex_free (mutex->m);
  free (mutex);
}

