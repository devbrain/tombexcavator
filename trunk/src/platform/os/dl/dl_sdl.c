struct shared_object_s_
{
  void* handle;
};
/* ------------------------------------------------------ */
shared_object_s* shared_object_open (const char* path)
{
  shared_object_s* result;
  void* h = SDL_LoadObject (path);

  if (h == NULL) 
    {
      return 0;
    }
  result = malloc (sizeof (shared_object_s));
  assert (result != NULL);
  result->handle = h;
  return result;
}
/* ------------------------------------------------------ */
void* shared_object_get_symbol (shared_object_s* so, 
				const char* name)
{
  if (so == NULL || so->handle == NULL)
    {
      return NULL;
    }
  return SDL_LoadFunction (so->handle, name); 
}
/* ------------------------------------------------------ */
void shared_object_close (shared_object_s* so)
{
  if (so == NULL)
    {
      return;
    }
  SDL_UnloadObject (so->handle);
  free (so);
}
