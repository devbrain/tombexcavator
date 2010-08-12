struct mutex_s_
{
  SDL_mutex* m;
};
/* ---------------------------------------- */
mutex_s* mutex_create (void)
{
  mutex_s* result = NULL;
  SDL_mutex* m = SDL_CreateMutex ();
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
  
  SDL_mutexP (mutex->m);
}
/* ---------------------------------------- */
void mutex_unlock (mutex_s* mutex)
{
  
  assert (mutex != NULL);
 
  SDL_mutexV (mutex->m);
}
/* ---------------------------------------- */
void mutex_destroy (mutex_s* mutex)
{
  if (mutex == NULL)
    {
      return;
    }
  SDL_DestroyMutex (mutex->m);
  free (mutex);
}

