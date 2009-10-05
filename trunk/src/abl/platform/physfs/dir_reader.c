#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "abl/platform/physfs/dir_reader.h"
#include "abl/unicode/unicode.h"
#include "thirdparty/predef/predef.h"

#if defined(PREDEF_PLATFORM_WIN32)
#if defined(PREDEF_COMPILER_VISUALC)
#define HAVE_DIRENT 0
#endif /* PREDEF_COMPILER_VISUALC */
#else /* PREDEF_PLATFORM_WIN32 */
#define HAVE_DIRENT 1
#endif /* PREDEF_PLATFORM_WIN32 */

#if (HAVE_DIRENT == 1)
#include <sys/types.h>
#include <dirent.h>
#else
#include "abl/platfrom/physfs/win32/dirent.h"
#include "abl/platform/physfs/win32/wdirent.c"
#endif



// Note that we are using PATH_MAX instead of _POSIX_PATH_MAX, since
// _POSIX_PATH_MAX is the *minimun* maximum value for PATH_MAX and is
// defined by POSIX as 256.
#if !defined (PATH_MAX)
#  if defined (_MAX_PATH)
#    define PATH_MAX _MAX_PATH
#  elif defined (MAX_PATH)
#    define PATH_MAX MAX_PATH
#  else /* !_MAX_PATH */
#    define PATH_MAX 1024
#  endif /* _MAX_PATH */
#endif /* !PATH_MAX */ 


struct _directory_s
{
#if defined(PREDEF_PLATFORM_WIN32)
  _WDIR *wdirp;
#else
  DIR *dirp;
#endif
#if defined(PREDEF_PLATFORM_WIN32)
  char utf8_buf [PATH_MAX*4];
#endif

};

directory_s* directory_open (const char* path)
{
  directory_s* dir;
#if defined(PREDEF_PLATFORM_WIN32)
  wchar_t *wpath;
#else
  char *utf8_path;
#endif

  if (!path)
    {
      return 0;
    }

  dir = malloc (sizeof (directory_s));

#if defined(PREDEF_PLATFORM_WIN32)
  wpath = utf8_to_utf16 (path, -1, NULL, NULL);

  if (wpath == NULL)
    {
      return NULL;
    }

  dir->wdirp = _wopendir (wpath);

  free (wpath);

  if (dir->wdirp)
    {
      return dir;
    }
#else /* PREDEF_PLATFORM_WIN32 */

  dir->dirp = opendir (path);

  if (dir->dirp)
    {
      return dir;
    }
#endif
  free (dir);
  return 0;
}
/* ---------------------------------------------------------------- */
const char* directory_read_name (directory_s* dir)
{
#if defined(PREDEF_PLATFORM_WIN32)
  char* utf8_name;
  struct _wdirent* wentry;
#else
  struct dirent* entry;
#endif /* PREDEF_PLATFORM_WIN32 */ 

  if (!dir)
    {
      return 0;
    }

#if defined(PREDEF_PLATFORM_WIN32)
  while (1)
    {
      wentry = _wreaddir (dir->wdirp);
      while (wentry 
	     && (0 == wcscmp (wentry->d_name, L".") ||
		 0 == wcscmp (wentry->d_name, L"..")))
	wentry = _wreaddir (dir->wdirp);

      if (wentry == NULL)
	{
	  return NULL;
	}

      utf8_name = utf16_to_utf8 (wentry->d_name, -1, NULL, NULL);

      if (utf8_name == NULL)
	{
	  continue;		/* Huh, impossible? Skip it anyway */
	}

      strcpy (dir->utf8_buf, utf8_name);

      free (utf8_name);

      return dir->utf8_buf;
    }
#else
  entry = readdir (dir->dirp);
  while (entry 
         && (0 == strcmp (entry->d_name, ".") ||
             0 == strcmp (entry->d_name, "..")))
    entry = readdir (dir->dirp);

  if (entry)
    {
      return entry->d_name;
    }
  else
    {
      return NULL;
    }
#endif /* PREDEF_PLATFORM_WIN32 */
}
/* ---------------------------------------------------------------- */
void directory_rewind (directory_s* dir)
{
  if (!dir)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  _wrewinddir (dir->wdirp);
#else
  rewinddir (dir->dirp);
#endif
}
/* ---------------------------------------------------------------- */
void directory_close (directory_s* dir)
{
  if (!dir)
    {
      return;
    }
#if defined(PREDEF_PLATFORM_WIN32)
  _wclosedir (dir->wdirp);
#else
  closedir (dir->dirp);
#endif
  free (dir);
}
