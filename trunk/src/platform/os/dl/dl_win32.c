typedef HMODULE shared_object_handle_t;

struct shared_object_s_
{
  shared_object_handle_t handle;
};
/* --------------------------------------------------------- */
shared_object_s* shared_object_open (const char* path)
{
  shared_object_s* result = 0;
  shared_object_handle_t         h;
  if (!path)
    {
      return 0;
    }
  
  h = LoadLibrary (path);
  if (!h)
    {
      return 0;
    }
  result = (shared_object_s*)malloc (sizeof (shared_object_s));
  result->handle = h;
  return result;
}
/* --------------------------------------------------------- */
void* shared_object_get_symbol (shared_object_s* so, const char* name)
{
  if (!so || !so->handle)
    {
      return 0;
    }
  return GetProcAddress (so->handle, name);
}
/* --------------------------------------------------------- */
void shared_object_close (shared_object_s* so)
{
  if (!so || !so->handle)
    {
      return;
    }
  FreeLibrary (so->handle);
  free (so);
}

