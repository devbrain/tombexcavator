struct os_mutex_s_
{
  SDL_mutex* m;
};
/* ---------------------------------------- */
os_mutex_s* os_mutex_create (void)
{
  os_mutex_s* result = NULL;
  SDL_mutex* m = SDL_CreateMutex ();
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
  
  SDL_mutexP (mutex->m);
}
/* ---------------------------------------- */
void os_mutex_unlock (os_mutex_s* mutex)
{
  
  assert (mutex != NULL);
 
  SDL_mutexV (mutex->m);
}
/* ---------------------------------------- */
void os_mutex_destroy (os_mutex_s* mutex)
{
  if (mutex == NULL)
    {
      return;
    }
  SDL_DestroyMutex (mutex->m);
  free (mutex);
}

