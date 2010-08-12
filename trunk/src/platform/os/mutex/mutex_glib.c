struct os_mutex_s_
{
  GMutex* m;
};
/* ---------------------------------------- */
os_mutex_s* os_mutex_create (void)
{
  os_mutex_s* result = NULL;
  GMutex* m = g_mutex_new ();
  if (m == NULL)
    {
      return NULL;
    }
  result = malloc (sizeof (os_mutex_s));
  result->m = m;
  return result;
}
/* ---------------------------------------- */
void os_mutex_lock (os_mutex_s* mutex)
{
  assert (mutex != NULL);
  g_mutex_lock (mutex->m);
}
/* ---------------------------------------- */
void os_mutex_unlock (os_mutex_s* mutex)
{
  assert (mutex != NULL);
  
  g_mutex_unlock (mutex->m);
}
/* ---------------------------------------- */
void os_mutex_destroy (os_mutex_s* mutex)
{
  if (mutex == NULL)
    {
      return;
    }
  g_mutex_free (mutex->m);
  free (mutex);
}

