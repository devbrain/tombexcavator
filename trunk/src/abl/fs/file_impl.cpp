#include "abl/fs/file_impl.hpp"
#include "abl/fs/path.hpp"

#include <assert.h>
#define poco_assert assert

#if defined(_WIN32)
#include "abl/exception.hpp"
#include "abl/string/string.hpp"
#include "abl/string/unicode_converter.hpp"
#include <windows.h>

namespace abl 
{
  class FileHandle
  {
  public:
    FileHandle (const std::string& path, 
		const std::wstring& upath, 
		DWORD access, 
		DWORD share, 
		DWORD disp)
    {
      _h = CreateFileW (upath.c_str (), access, share, 0, disp, 0, 0);
      if  (_h == INVALID_HANDLE_VALUE)
	{
	  file_impl_c::handleLastErrorImpl (path);
	}
    }
	
    ~FileHandle ()
    {
      if  (_h != INVALID_HANDLE_VALUE) CloseHandle (_h);
    }
	
    HANDLE get () const
    {
      return _h;
    }
	
  private:
    HANDLE _h;
  };


  file_impl_c::file_impl_c ()
  {
  }


  file_impl_c::file_impl_c (const std::string& path): _path (path)
  {
    std::string::size_type n = _path.size ();
    if  (n > 1 &&  (_path[n - 1] == '\\' || _path[n - 1] == '/') && ! ( (n == 3 && _path[1]==':')))
      {
	_path.resize (n - 1);
      }
    unicode_converter_c::to_utf16 (_path, _upath);
  }


  file_impl_c::~file_impl_c ()
  {
  }


  void file_impl_c::swapImpl (file_impl_c& file)
  {
    std::swap (_path, file._path);
    std::swap (_upath, file._upath);
  }


  void file_impl_c::setPathImpl (const std::string& path)
  {
    _path = path;
    std::string::size_type n = _path.size ();
    if  (n > 1 &&  (_path[n - 1] == '\\' || _path[n - 1] == '/') && ! ( (n == 3 && _path[1]==':')))
      {
	_path.resize (n - 1);
      }
    unicode_converter_c::to_utf16 (_path, _upath);
  }


  bool file_impl_c::existsImpl () const
  {
    poco_assert  (!_path.empty ());

    DWORD attr = GetFileAttributesW (_upath.c_str ());
    if  (attr == 0xFFFFFFFF)
      {
	switch  (GetLastError ())
	  {
	  case ERROR_FILE_NOT_FOUND:
	  case ERROR_PATH_NOT_FOUND:
	  case ERROR_NOT_READY:
	  case ERROR_INVALID_DRIVE:
	    return false;
	  default:
	    handleLastErrorImpl (_path);
	  }
      }
    return true;
  }


  bool file_impl_c::canReadImpl () const
  {
    poco_assert  (!_path.empty ());
	
    DWORD attr = GetFileAttributesW (_upath.c_str ());
    if  (attr == 0xFFFFFFFF)
      {
	switch  (GetLastError ())
	  {
	  case ERROR_ACCESS_DENIED:
	    return false;
	  default:
	    handleLastErrorImpl (_path);
	  }
      }
    return true;
  }


  bool file_impl_c::canWriteImpl () const
  {
    poco_assert  (!_path.empty ());
	
    DWORD attr = GetFileAttributesW (_upath.c_str ());
    if  (attr == 0xFFFFFFFF)
      handleLastErrorImpl (_path);
    return  (attr & FILE_ATTRIBUTE_READONLY) == 0;
  }


  bool file_impl_c::canExecuteImpl () const
  {
    path_c p (_path);
    return icompare (p.get_extension (), "exe") == 0;
  }


  bool file_impl_c::isFileImpl() const
  {
    return !isDirectoryImpl () && !isDeviceImpl ();
  }


  bool file_impl_c::isDirectoryImpl () const
  {
    poco_assert  (!_path.empty ());

    DWORD attr = GetFileAttributesW (_upath.c_str ());
    if  (attr == 0xFFFFFFFF)
      handleLastErrorImpl (_path);
    return  (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;
  }


  bool file_impl_c::isLinkImpl () const
  {
    return false;
  }


  bool file_impl_c::isDeviceImpl () const
  {
    poco_assert  (!_path.empty ());

    FileHandle fh (_path, _upath, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);
    DWORD type = GetFileType (fh.get ());
    if  (type == FILE_TYPE_CHAR)
      return true;
    else if  (type == FILE_TYPE_UNKNOWN && GetLastError () != NO_ERROR)
      handleLastErrorImpl (_path);
    return false;
  }


  bool file_impl_c::isHiddenImpl () const
  {
    poco_assert  (!_path.empty ());

    DWORD attr = GetFileAttributesW (_upath.c_str ());
    if  (attr == 0xFFFFFFFF)
      handleLastErrorImpl (_path);
    return  (attr & FILE_ATTRIBUTE_HIDDEN) != 0;
  }


  time_stamp_c file_impl_c::createdImpl () const
  {
    poco_assert  (!_path.empty ());

    WIN32_FILE_ATTRIBUTE_DATA fad;
    if  (GetFileAttributesExW (_upath.c_str (), GetFileExInfoStandard, &fad) == 0) 
      handleLastErrorImpl (_path);
    return time_stamp_c::from_file_time_np (fad.ftCreationTime.dwLowDateTime, fad.ftCreationTime.dwHighDateTime);
  }


  time_stamp_c file_impl_c::getLastModifiedImpl () const
  {
    poco_assert  (!_path.empty ());

    WIN32_FILE_ATTRIBUTE_DATA fad;
    if  (GetFileAttributesExW (_upath.c_str (), GetFileExInfoStandard, &fad) == 0) 
      handleLastErrorImpl (_path);
    return time_stamp_c::from_file_time_np (fad.ftLastWriteTime.dwLowDateTime, fad.ftLastWriteTime.dwHighDateTime);
  }


  void file_impl_c::setLastModifiedImpl (const time_stamp_c& ts)
  {
    poco_assert  (!_path.empty ());

    UInt32 low;
    UInt32 high;
    ts.to_file_time_np (low, high);
    FILETIME ft;
    ft.dwLowDateTime  = low;
    ft.dwHighDateTime = high;
    FileHandle fh (_path, _upath, FILE_ALL_ACCESS, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);
    if  (SetFileTime (fh.get (), 0, &ft, &ft) == 0)
      handleLastErrorImpl (_path);
  }


  file_impl_c::FileSizeImpl file_impl_c::getSizeImpl () const
  {
    poco_assert  (!_path.empty ());

    WIN32_FILE_ATTRIBUTE_DATA fad;
    if  (GetFileAttributesExW (_upath.c_str (), GetFileExInfoStandard, &fad) == 0) 
      handleLastErrorImpl (_path);
    LARGE_INTEGER li;
    li.LowPart  = fad.nFileSizeLow;
    li.HighPart = fad.nFileSizeHigh;
    return li.QuadPart;
  }


  void file_impl_c::setSizeImpl (FileSizeImpl size)
  {
    poco_assert  (!_path.empty ());

    FileHandle fh (_path, _upath, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, OPEN_EXISTING);
    LARGE_INTEGER li;
    li.QuadPart = size;
    if  (SetFilePointer (fh.get (), li.LowPart, &li.HighPart, FILE_BEGIN) == -1)
      handleLastErrorImpl (_path);
    if  (SetEndOfFile (fh.get ()) == 0) 
      handleLastErrorImpl (_path);
  }


  void file_impl_c::setWriteableImpl (bool flag)		
  {
    poco_assert  (!_path.empty ());

    DWORD attr = GetFileAttributesW (_upath.c_str ());
    if  (attr == -1)
      handleLastErrorImpl (_path);
    if  (flag)
      attr &= ~FILE_ATTRIBUTE_READONLY;
    else
      attr |= FILE_ATTRIBUTE_READONLY;
    if  (SetFileAttributesW (_upath.c_str (), attr) == 0)
      handleLastErrorImpl (_path);
  }


  void file_impl_c::setExecutableImpl (bool flag)
  {
    // not supported
  }


  void file_impl_c::copyToImpl (const std::string& path) const
  {
    poco_assert  (!_path.empty ());

    std::wstring upath;
    unicode_converter_c::to_utf16 (path, upath);
    if  (CopyFileW (_upath.c_str (), upath.c_str (), FALSE) != 0)
      {
	file_impl_c copy (path);
	copy.setWriteableImpl (true);
      }
    else handleLastErrorImpl (_path);
  }


  void file_impl_c::renameToImpl (const std::string& path)
  {
    poco_assert  (!_path.empty ());

    std::wstring upath;
    unicode_converter_c::to_utf16 (path, upath);
    if  (MoveFileW (_upath.c_str (), upath.c_str ()) == 0) 
      handleLastErrorImpl (_path);
  }


  void file_impl_c::removeImpl ()
  {
    poco_assert  (!_path.empty ());

    if  (isDirectoryImpl ())
      {
	if  (RemoveDirectoryW (_upath.c_str ()) == 0) 
	  handleLastErrorImpl (_path);
      }
    else
      {
	if  (DeleteFileW (_upath.c_str ()) == 0)
	  handleLastErrorImpl (_path);
      }
  }


  bool file_impl_c::createFileImpl ()
  {
    poco_assert  (!_path.empty ());

    HANDLE hFile = CreateFileW (_upath.c_str (), GENERIC_WRITE, 0, 0, CREATE_NEW, 0, 0);
    if  (hFile != INVALID_HANDLE_VALUE)
      {
	CloseHandle (hFile);
	return true;
      }
    else if  (GetLastError () == ERROR_FILE_EXISTS)
      return false;
    else
      handleLastErrorImpl (_path);
    return false;
  }


  bool file_impl_c::createDirectoryImpl ()
  {
    poco_assert  (!_path.empty ());
	
    if  (existsImpl () && isDirectoryImpl ())
      return false;
    if  (CreateDirectoryW (_upath.c_str (), 0) == 0)
      handleLastErrorImpl (_path);
    return true;
  }


  void file_impl_c::handleLastErrorImpl (const std::string& path)
  {
    switch  (GetLastError ())
      {
      case ERROR_FILE_NOT_FOUND:
	throw file_not_found_exception_c (path);
      case ERROR_PATH_NOT_FOUND:
      case ERROR_BAD_NETPATH:
      case ERROR_CANT_RESOLVE_FILENAME:
      case ERROR_INVALID_DRIVE:
	throw path_not_found_exception_c (path);
      case ERROR_ACCESS_DENIED:
	throw file_access_denied_exception_c (path);
      case ERROR_ALREADY_EXISTS:
      case ERROR_FILE_EXISTS:
	throw file_exists_exception_c (path);
      case ERROR_INVALID_NAME:
      case ERROR_DIRECTORY:
      case ERROR_FILENAME_EXCED_RANGE:
      case ERROR_BAD_PATHNAME:
	throw path_syntax_exception_c (path);
      case ERROR_FILE_READ_ONLY:
	throw file_read_only_exception_c (path);
      case ERROR_CANNOT_MAKE:
	throw create_file_exception_c (path);
      case ERROR_DIR_NOT_EMPTY:
	throw file_exception_c ("directory not empty", path);
      case ERROR_WRITE_FAULT:
	throw write_file_exception_c (path);
      case ERROR_READ_FAULT:
	throw read_file_exception_c (path);
      case ERROR_SHARING_VIOLATION:
	throw file_exception_c ("sharing violation", path);
      case ERROR_LOCK_VIOLATION:
	throw file_exception_c ("lock violation", path);
      case ERROR_HANDLE_EOF:
	throw read_file_exception_c ("EOF reached", path);
      case ERROR_HANDLE_DISK_FULL:
      case ERROR_DISK_FULL:
	throw write_file_exception_c ("disk is full", path);
      case ERROR_NEGATIVE_SEEK:
	throw file_exception_c ("negative seek", path);
      default:
	throw file_exception_c (path);
      }
  }
} // ns abl 
#else

#include "abl/buffer.hpp"
#include "abl/exception.hpp"
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <utime.h>
#include <cstring>


  namespace abl 
  {


    file_impl_c::file_impl_c ()
    {
    }


    file_impl_c::file_impl_c (const std::string& path): _path (path)
    {
      std::string::size_type n = _path.size ();
      if  (n > 1 && _path[n - 1] == '/')
	_path.resize (n - 1);
    }


    file_impl_c::~file_impl_c ()
    {
    }


    void file_impl_c::swapImpl (file_impl_c& file)
    {
      std::swap (_path, file._path);
    }


    void file_impl_c::setPathImpl (const std::string& path)
    {
      _path = path;
      std::string::size_type n = _path.size ();
      if  (n > 1 && _path[n - 1] == '/')
	_path.resize (n - 1);
    }


    bool file_impl_c::existsImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      return stat (_path.c_str (), &st) == 0;
    }


    bool file_impl_c::canReadImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	{
	  if  (geteuid () == 0)
	    return true;
	  else if  (st.st_uid == geteuid ())
	    return  (st.st_mode & S_IRUSR) != 0;
	  else if  (st.st_gid == getegid ())
	    return  (st.st_mode & S_IRGRP) != 0;
	  else
	    return  (st.st_mode & S_IROTH) != 0;
	}
      else handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::canWriteImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	{
	  if  (geteuid () == 0)
	    return true;
	  else if  (st.st_uid == geteuid ())
	    return  (st.st_mode & S_IWUSR) != 0;
	  else if  (st.st_gid == getegid ())
	    return  (st.st_mode & S_IWGRP) != 0;
	  else
	    return  (st.st_mode & S_IWOTH) != 0;
	}
      else handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::canExecuteImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	{
	  if  (st.st_uid == geteuid () || geteuid () == 0)
	    return  (st.st_mode & S_IXUSR) != 0;
	  else if  (st.st_gid == getegid ())
	    return  (st.st_mode & S_IXGRP) != 0;
	  else
	    return  (st.st_mode & S_IXOTH) != 0;
	}
      else handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::isFileImpl() const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return S_ISREG (st.st_mode);
      else
	handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::isDirectoryImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return S_ISDIR (st.st_mode);
      else
	handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::isLinkImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (lstat (_path.c_str (), &st) == 0)
	return S_ISLNK (st.st_mode);
      else
	handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::isDeviceImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return S_ISCHR (st.st_mode) || S_ISBLK (st.st_mode);
      else
	handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::isHiddenImpl () const
    {
      poco_assert  (!_path.empty ());
      Path p (_path);
      p.makeFile ();

      return p.getFileName ()[0] == '.';
    }


    time_stamp_c file_impl_c::createdImpl () const
    {
      poco_assert  (!_path.empty ());

#if defined (__APPLE__) && defined (st_birthtime) && !defined (POCO_NO_STAT64) // st_birthtime is available only on 10.5
      struct stat64 st;
      if  (stat64 (_path.c_str (), &st) == 0)
	return time_stamp_c::fromEpochTime (st.st_birthtime);
#elif defined (__FreeBSD__)
      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return time_stamp_c::fromEpochTime (st.st_birthtime);
#else
      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return time_stamp_c::fromEpochTime (st.st_ctime);
#endif 
      else
	handleLastErrorImpl (_path);
      return 0;
    }


    time_stamp_c file_impl_c::getLastModifiedImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return time_stamp_c::fromEpochTime (st.st_mtime);
      else
	handleLastErrorImpl (_path);
      return 0;
    }


    void file_impl_c::setLastModifiedImpl (const time_stamp_c& ts)
    {
      poco_assert  (!_path.empty ());

      struct utimbuf tb;
      tb.actime  = ts.epochTime ();
      tb.modtime = ts.epochTime ();
      if  (utime (_path.c_str (), &tb) != 0)
	handleLastErrorImpl (_path);
    }


    file_impl_c::FileSizeImpl file_impl_c::getSizeImpl () const
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) == 0)
	return st.st_size;
      else
	handleLastErrorImpl (_path);
      return 0;
    }


    void file_impl_c::setSizeImpl (FileSizeImpl size)
    {
      poco_assert  (!_path.empty ());

      if  (truncate (_path.c_str (), size) != 0)
	handleLastErrorImpl (_path);
    }


    void file_impl_c::setWriteableImpl (bool flag)
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) != 0) 
	handleLastErrorImpl (_path);
      mode_t mode;
      if  (flag)
	{
	  mode = st.st_mode | S_IWUSR;
	}
      else
	{
	  mode_t wmask = S_IWUSR | S_IWGRP | S_IWOTH;
	  mode = st.st_mode & ~wmask;
	}
      if  (chmod (_path.c_str (), mode) != 0) 
	handleLastErrorImpl (_path);
    }


    void file_impl_c::setExecutableImpl (bool flag)
    {
      poco_assert  (!_path.empty ());

      struct stat st;
      if  (stat (_path.c_str (), &st) != 0) 
	handleLastErrorImpl (_path);
      mode_t mode;
      if  (flag)
	{
	  mode = st.st_mode | S_IXUSR;
	}
      else
	{
	  mode_t wmask = S_IXUSR | S_IXGRP | S_IXOTH;
	  mode = st.st_mode & ~wmask;
	}
      if  (chmod (_path.c_str (), mode) != 0) 
	handleLastErrorImpl (_path);
    }


    void file_impl_c::copyToImpl (const std::string& path) const
    {
      poco_assert  (!_path.empty ());

      int sd = open (_path.c_str (), O_RDONLY);
      if  (sd == -1) handleLastErrorImpl (_path);

      struct stat st;
      if  (fstat (sd, &st) != 0) 
	{
	  close (sd);
	  handleLastErrorImpl (_path);
	}
      const long blockSize = st.st_blksize;

      int dd = open (path.c_str (), O_CREAT | O_TRUNC | O_WRONLY, st.st_mode & S_IRWXU);
      if  (dd == -1)
	{
	  close (sd);
	  handleLastErrorImpl (path);
	}
      Buffer<char> buffer (blockSize);
      try
	{
	  int n;
	  while  ( (n = read (sd, buffer.begin (), blockSize)) > 0)
	    {
	      if  (write (dd, buffer.begin (), n) != n) 
		handleLastErrorImpl (path);
	    }
	  if  (n < 0)
	    handleLastErrorImpl (_path);
	}
      catch  (...)
	{
	  close (sd);
	  close (dd);
	  throw;
	}
      close (sd);
      if  (fsync (dd) != 0) 
	{
	  close (dd);
	  handleLastErrorImpl (path);
	}
      if  (close (dd) != 0)
	handleLastErrorImpl (path);
    }


    void file_impl_c::renameToImpl (const std::string& path)
    {
      poco_assert  (!_path.empty ());

      if  (rename (_path.c_str (), path.c_str ()) != 0)
	handleLastErrorImpl (_path);
    }


    void file_impl_c::removeImpl ()
    {
      poco_assert  (!_path.empty ());

      int rc;
      if  (!isLinkImpl () && isDirectoryImpl ())
	rc = rmdir (_path.c_str ());
      else
	rc = unlink (_path.c_str ());
      if  (rc) handleLastErrorImpl (_path);
    }


    bool file_impl_c::createFileImpl ()
    {
      poco_assert  (!_path.empty ());
	
      int n = open (_path.c_str (), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
      if  (n != -1)
	{
	  close (n);
	  return true;
	}
      if  (n == -1 && errno == EEXIST)
	return false;
      else
	handleLastErrorImpl (_path);
      return false;
    }


    bool file_impl_c::createDirectoryImpl ()
    {
      poco_assert  (!_path.empty ());

      if  (existsImpl () && isDirectoryImpl ())
	return false;
      if  (mkdir (_path.c_str (), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) 
	handleLastErrorImpl (_path);
      return true;
    }


    void file_impl_c::handleLastErrorImpl (const std::string& path)
    {
      switch  (errno)
	{
	case EIO:
	  throw IO_exception_c (path);
	case EPERM:
	  throw file_access_denied_exception_c ("insufficient permissions", path);
	case EACCES:
	  throw file_access_denied_exception_c (path);
	case ENOENT:
	  throw file_not_found_exception_c (path);
	case ENOTDIR:
	  throw open_file_exception_c ("not a directory", path);
	case EISDIR:
	  throw open_file_exception_c ("not a file", path);
	case EROFS:
	  throw file_read_only_exception_c (path);
	case EEXIST:
	  throw file_exists_exception_c (path);
	case ENOSPC:
	  throw file_exception_c ("no space left on device", path);
	case EDQUOT:
	  throw file_exception_c ("disk quota exceeded", path);
#if !defined (_AIX)
	case ENOTEMPTY:
	  throw file_exception_c ("directory not empty", path);
#endif
	case ENAMETOOLONG:
	  throw path_syntax_exception_c (path);
	case ENFILE:
	case EMFILE:
	  throw file_exception_c ("too many open files", path);
	default:
	  throw file_exception_c (std::strerror (errno), path);
	}
    }
 

#endif

