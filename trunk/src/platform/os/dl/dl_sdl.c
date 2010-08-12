struct os_shared_object_s_
{
  void* handle;
};
/* ------------------------------------------------------ */
os_shared_object_s* os_shared_object_open (const char* path)
{
  os_shared_object_s* result;
  void* h = SDL_LoadObject (path);

  if (h == NULL) 
    {
      return 0;
    }
  result = malloc (sizeof (os_shared_object_s));
  assert (result != NULL);
  result->handle = h;
  return result;
}
/* ------------------------------------------------------ */
void* os_shared_object_get_symbol (os_shared_object_s* so, 
				const char* name)
{
  if (so == NULL || so->handle == NULL)
    {
      return NULL;
    }
  return SDL_LoadFunction (so->handle, name); 
}
/* ------------------------------------------------------ */
void os_shared_object_close (os_shared_object_s* so)
{
  if (so == NULL)
    {
      return;
    }
  SDL_UnloadObject (so->handle);
  free (so);
}
