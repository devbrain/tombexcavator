#include "platform/os_error.h"
#include "thirdparty/predef/predef.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>


#if defined(PREDEF_PLATFORM_WIN32)
#include <windows.h>
#elif defined(PREDEF_PLATFORM_UNIX)
#include <errno.h>
#else
#error "Unknown platform"
#endif


struct os_error_s_
{
  os_error_id_t m_error_id;
  char*         m_string;
};

/* --------------------------------------------------- */

os_error_id_t  get_os_error_id (const os_error_s* os_error)
{
  if (os_error == NULL)
    {
      return eNO_ERROR;
    }
  return os_error->m_error_id;
}

/* --------------------------------------------------- */

const char* get_os_error_string (const os_error_s* os_error)
{
  if (os_error == NULL)
    {
      return NULL;
    }
  return os_error->m_string;
}

/* --------------------------------------------------- */

void free_os_error (os_error_s* os_error)
{
  if (os_error != NULL)
    {
      free (os_error->m_string);
      free (os_error);
    }
}

/* --------------------------------------------------- */
static os_error_id_t error_id_from_last_error (void)
{
#if defined(PREDEF_PLATFORM_UNIX)
  switch (errno)
    {
    case EACCES:
    case EPERM:
    case ETXTBSY:
      return eACCESS_DENIED;
    case EEXIST:
      return ePATHNAME_EXISTS;
    case EISDIR:
      return ePATHNAME_IS_DIR;
    case EMFILE:
    case ENFILE:
      return eNO_DESCRIPTORS;
    case ENODEV:
    case ENOENT:
      return eFILE_NOT_FOUND;
    case ENOMEM:
    case ENOSPC:
      return eNO_SPACE_LEFT;
    case ENOTDIR:
      return ePATHNAME_NOT_DIR;
    default:
      return eUNKNOWN_ERROR;
    }
#elif defined(PREDEF_PLATFORM_WIN32)
  #error "Implement Me"
#else
#error "Unknown platform"
#endif
}

/* --------------------------------------------------- */
static void create_error_string (os_error_s* os_error)
{
  
  
#if defined(PREDEF_PLATFORM_UNIX)
  const char* serr = strerror (errno);
  size_t serr_len  = strlen (serr);
  
  assert (os_error != NULL);

  os_error->m_string = malloc (serr_len + 1);
  assert (os_error->m_string);
  
  memcpy (os_error->m_string, serr, serr_len);
  os_error->m_string [serr_len] = 0;

#elif defined(PREDEF_PLATFORM_WIN32)
#error "Implement Me"
#else
#error "Unknown platform"
#endif
}
/* --------------------------------------------------- */
os_error_s* create_os_error_from_last_error (void)
{
  os_error_s* result = malloc (sizeof (os_error_s));
  assert (result != NULL);
  result->m_error_id = error_id_from_last_error ();
  create_error_string (result);
  return result;
}
