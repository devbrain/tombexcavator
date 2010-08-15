typedef HMODULE shared_object_handle_t;

struct os_shared_object_s_
{
  shared_object_handle_t handle;
};
/* --------------------------------------------------------- */
os_shared_object_s* os_shared_object_open (const char* path)
{
  os_shared_object_s* result = 0;
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
  result = (os_shared_object_s*)malloc (sizeof (os_shared_object_s));
  result->handle = h;
  return result;
}
/* --------------------------------------------------------- */
void* os_shared_object_get_symbol (os_shared_object_s* so, const char* name)
{
  if (!so || !so->handle)
    {
      return 0;
    }
  return (void*) GetProcAddress (so->handle, name);
}
/* --------------------------------------------------------- */
void os_shared_object_close (os_shared_object_s* so)
{
  if (!so || !so->handle)
    {
      return;
    }
  FreeLibrary (so->handle);
  free (so);
}

