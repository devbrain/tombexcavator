

struct os_file_descriptor_s_
{
  HANDLE fd;
};
/* ------------------------------------------------------------ */

STATIC_ASSERT (wchar_t,   (sizeof (wchar_t) == sizeof (uint16_t)));

static HANDLE create_file (const char* path, int flag)
{
	
}

os_file_descriptor_s* os_open_file_for_read (const char* path,
					     os_error_s** error)
{
  
  os_file_descriptor_s* result = NULL;

  int fd = open (path, O_RDONLY);

  if (fd == -1)
    {
      SET_ERROR(error);
      return NULL;
    }
  result = malloc (sizeof (os_file_descriptor_s));
  result->fd = fd;
  return result;
}
/* ------------------------------------------------------------ */
os_file_descriptor_s* os_create_file (const char* path,
				      os_error_s** error)
{
  os_file_descriptor_s* result = NULL;

  int fd = open (path, 
		 O_RDWR | O_CREAT | O_TRUNC, 
		 S_IWUSR | S_IRUSR | S_IRGRP );

  if (fd == -1)
    {
      SET_ERROR(error);
      return NULL;
    }
  result = malloc (sizeof (os_file_descriptor_s));
  result->fd = fd;
  return result;
}
/* ------------------------------------------------------------ */
os_file_descriptor_s* os_open_file_for_append (const char* path,
					       os_error_s** error)
{
  os_file_descriptor_s* result = NULL;

  int fd = open (path, 
		 O_RDWR  | O_CREAT, 
		 S_IWUSR | S_IRUSR | S_IRGRP );

  if (fd == -1)
    {
      SET_ERROR(error);
      return NULL;
    }
  result = malloc (sizeof (os_file_descriptor_s));
  result->fd = fd;
  return result;
}
/* ------------------------------------------------------------ */
int os_flush_file (os_file_descriptor_s* fd,
		   os_error_s** error)
{
  assert (fd != NULL);
  if (fsync (fd->fd) == 0)
    {
      return 1;
    }
  SET_ERROR(error);
  return 0;
}
/* ------------------------------------------------------------ */
int os_close_file (os_file_descriptor_s* fd,
		   os_error_s** error)
{
  int f;
  assert (fd != NULL);
  f = fd->fd;
  free (fd);
  if (close (f))
    {
      SET_ERROR(error);
      return 0;
    }
  return 1;
}
/* ------------------------------------------------------------ */
int os_write_to_file (os_file_descriptor_s* fd,
		      const void* buff,
		      size_t      size,
		      size_t*     written,
		      os_error_s** error)
{
  ssize_t rc;
  assert (fd != NULL);
  rc = write (fd->fd, buff, size);
  if (written != NULL)
    {
      *written = (rc < 0) ? 0 : (size_t)rc;
    }
  if (rc < 0)
    {
      SET_ERROR(error);
      return 0;
    }
  return 1;
}
/* ------------------------------------------------------------ */
int os_read_from_file (os_file_descriptor_s* fd,
		       void*   buff,
		       size_t  size,
		       size_t* has_bytes,
		       os_error_s** error)
{
  ssize_t rc;
  assert (fd != NULL);
  rc = read (fd->fd, buff, size);
  if (has_bytes != NULL)
    {
      *has_bytes = (rc < 0) ? 0 : (size_t)rc;
    }
  if (rc < 0)
    {
      SET_ERROR(error);
      return 0;
    }
  return 1;
}
/* ------------------------------------------------------------ */
int os_path_exists (const char* path)
{
  int rc;
  struct stat st;
  rc = stat (path, &st);
  if (rc)
    {
      return 0;
    }
  return 1;
}
/* ------------------------------------------------------------ */
os_file_offset_t os_file_size (os_file_descriptor_s* fd,
			       os_error_s** error)
{
  
  off_t curr;
  off_t r;
  assert (fd != NULL);

  curr = lseek (fd->fd, 0, SEEK_CUR);
  if (curr == (off_t)-1)
    {
      SET_ERROR (error);
      return 0;
    }
  r = lseek (fd->fd, 0, SEEK_END);
  if (r == (off_t)-1)
    {
      SET_ERROR (error);
      return 0;
    }
  return (os_file_offset_t)r;
}
/* ------------------------------------------------------------ */
os_file_offset_t os_current_file_position (os_file_descriptor_s* fd,
					   os_error_s** error)
{
  off_t curr;
  assert (fd != NULL);

  curr = lseek (fd->fd, 0, SEEK_CUR);
  if (curr == (off_t)-1)
    {
      SET_ERROR (error);
      return 0;
    }
  return (os_file_offset_t)curr;
}
/* ------------------------------------------------------------ */
int os_set_current_file_position (os_file_descriptor_s* fd,
				  os_file_offset_t      offset,
				  os_seek_direction_t   direction)
{
  off_t curr;
  int whence;
  assert (fd != NULL);

  switch (direction)
    {
    case eFROM_START:
      whence = SEEK_SET;
      break;
    case eFROM_END:
      whence = SEEK_END;
      break;
    case eFROM_CURRENT:
      whence = SEEK_CUR;
      break;
    };
  curr = lseek (fd->fd, (off_t)offset, whence);
  if (curr == (off_t)-1)
    {
      return 0;
    }
  return 1;
}
/* ------------------------------------------------------------ */
int os_path_is_link (const char* path)
{
  int rc;
  struct stat st;
  rc = stat (path, &st);
  if (rc)
    {
      return 0;
    }
  return S_ISLNK (st.st_mode);
}
/* ------------------------------------------------------------ */
int os_path_is_file (const char* path)
{
  int rc;
  struct stat st;
  rc = stat (path, &st);
  if (rc)
    {
      return 0;
    }
  return S_ISREG (st.st_mode);
}
/* ------------------------------------------------------------ */
int os_path_is_directory (const char* path)
{
  int rc;
  struct stat st;
  rc = stat (path, &st);
  if (rc)
    {
      return 0;
    }
  return S_ISDIR (st.st_mode);
}
/* ------------------------------------------------------------ */
void os_enumerate_files (const char *dirname,
			 int omit_sym_links,
			 enum_files_callback_t callback,
			 const char *origdir,
			 void *callbackdata)
{
  DIR *dir;
  struct dirent *ent;
  size_t bufsize = 0;
  char *buf = NULL;
  size_t dlen = 0;
  size_t len;
  char *p;

  if (omit_sym_links)  
    {
      dlen = strlen(dirname);
      bufsize = dlen + 256;
      buf = (char *) malloc (bufsize);
      if (buf == NULL)
	{
	  return;
	}
      strcpy(buf, dirname);
      if (buf[dlen - 1] != '/')
        {
	  buf[dlen++] = '/';
	  buf[dlen] = '\0';
        } /* if */
    } /* if */
  errno = 0;
  dir = opendir(dirname);
  if (dir == NULL)
    {
      free (buf);
      return;
    } /* if */

  while ((ent = readdir(dir)) != NULL)
    {
      if (strcmp(ent->d_name, ".") == 0)
	continue;
      
      if (strcmp(ent->d_name, "..") == 0)
	continue;
      
      if (omit_sym_links)
        {
	  
	  len = strlen(ent->d_name) + dlen + 1;
	  if (len > bufsize)
            {
	      p = (char *) realloc(buf, len);
	      if (p == NULL)
		continue;
	      buf = p;
	      bufsize = len;
            } /* if */
	  
	  strcpy(buf + dlen, ent->d_name);
	  if (os_path_is_link (buf))
	    continue;
        } /* if */
      callback(callbackdata, origdir, ent->d_name);
    } /* while */
  
  free (buf);
  closedir (dir);
}
