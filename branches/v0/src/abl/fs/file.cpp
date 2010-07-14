#include "abl/fs/file.hpp"
#include "abl/fs/path.hpp"
#include "abl/fs/directory_iterator.hpp"

#include <assert.h>
#define poco_check_ptr assert

namespace abl 
{
  file_c::file_c ()
  {
  }
  // ---------------------------------------------------------------------
  file_c::file_c (const std::string& path)
    : file_impl_c (path)
  {
  }
  // ---------------------------------------------------------------------
  file_c::file_c (const char* path)
    : file_impl_c (std::string (path))
  {
  }
  // ---------------------------------------------------------------------
  file_c::file_c (const path_c& path)
    : file_impl_c (path.to_string ())
  {
  }
  // ---------------------------------------------------------------------
  file_c::file_c (const file_c& file)
    : file_impl_c (file.getPathImpl ())
  {
  }
  // ---------------------------------------------------------------------
  file_c::~file_c ()
  {
  }
  // ---------------------------------------------------------------------
  file_c& file_c::operator = (const file_c& file)
  {
    setPathImpl (file.getPathImpl ());
    return *this;
  }
  // ---------------------------------------------------------------------
  file_c& file_c::operator =  (const std::string& path)
  {
    setPathImpl (path);
    return *this;
  }
  // ---------------------------------------------------------------------
  file_c& file_c::operator =  (const char* path)
  {
    poco_check_ptr  (path);
    setPathImpl (path);
    return *this;
  }
  // ---------------------------------------------------------------------
  file_c& file_c::operator =  (const path_c& path)
  {
    setPathImpl (path.to_string ());
    return *this;
  }
  // ---------------------------------------------------------------------
  void file_c::swap (file_c& file)
  {
    swapImpl (file);
  }
  // ---------------------------------------------------------------------
  bool file_c::exists () const
  {
    return existsImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::can_read () const
  {
    return canReadImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::can_write () const
  {
    return canWriteImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::can_execute () const
  {
    return canExecuteImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::is_file () const
  {
    return isFileImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::is_directory () const
  {
    return isDirectoryImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::is_link () const
  {
    return isLinkImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::is_device () const
  {
    return isDeviceImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::is_hidden () const
  {
    return isHiddenImpl ();
  }
  // ---------------------------------------------------------------------
  time_stamp_c file_c::created () const
  {
    return createdImpl ();
  }
  // ---------------------------------------------------------------------
  time_stamp_c file_c::get_last_modified () const
  {
    return getLastModifiedImpl ();
  }
  // ---------------------------------------------------------------------
  void file_c::set_last_modified (const time_stamp_c& ts)
  {
    setLastModifiedImpl (ts);
  }
  // ---------------------------------------------------------------------
  file_c::file_size_t file_c::get_size () const
  {
    return getSizeImpl ();
  }
  // ---------------------------------------------------------------------
  void file_c::set_size (file_size_t size)
  {
    setSizeImpl (size);
  }
  // ---------------------------------------------------------------------
  void file_c::set_writeable (bool flag)
  {
    setWriteableImpl (flag);
  }
  // ---------------------------------------------------------------------
  void file_c::set_read_only (bool flag)
  {
    setWriteableImpl (!flag);
  }
  // ---------------------------------------------------------------------
  void file_c::set_executable (bool flag)
  {
    setExecutableImpl (flag);
  }
  // ---------------------------------------------------------------------
  void file_c::copy_to (const std::string& path) const
  {
    path_c src (getPathImpl ());
    path_c dest (path);
    file_c destfile_c (path);
    if  ( (destfile_c.exists () && destfile_c.is_directory ()) || dest.is_directory ())
      {
	dest.make_directory ();
	dest.set_file_name (src.get_file_name ());
      }
    if  (is_directory ())
      _copy_directory (dest.to_string ());
    else
      copyToImpl (dest.to_string ());
  }
  // ---------------------------------------------------------------------
  void file_c::_copy_directory (const std::string& path) const
  {
    file_c target (path);
    target.create_directories ();

    path_c src (getPathImpl ());
    src.make_file ();
    directory_iterator_c it (src);
    directory_iterator_c end;
    for  (; it != end; ++it)
      {
	it->copy_to (path);
      }
  }
  // ---------------------------------------------------------------------
  void file_c::move_to (const std::string& path)
  {
    copy_to (path);
    remove (true);
    setPathImpl (path);
  }
  // ---------------------------------------------------------------------
  void file_c::rename_to (const std::string& path)
  {
    renameToImpl (path);
    setPathImpl (path);
  }
  // ---------------------------------------------------------------------
  void file_c::remove (bool recursive)
  {
    if  (recursive && !is_link () && is_directory ())
      {
	std::vector<file_c> files;
	list (files);
	for  (std::vector<file_c>::iterator it = files.begin (); it != files.end (); ++it)
	  {
	    it->remove (true);
	  }
      }
    removeImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::create_file ()
  {
    return createFileImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::create_directory ()
  {
    return createDirectoryImpl ();
  }
  // ---------------------------------------------------------------------
  void file_c::create_directories ()
  {
    if  (!exists ())
      {
	path_c p (getPathImpl ());
	p.make_directory ();
	if  (p.depth () > 1)
	  {
	    p.make_parent ();
	    file_c f (p);
	    f.create_directories ();
	  }
	createDirectoryImpl ();
      }
  }
  // ---------------------------------------------------------------------
  void file_c::list (std::vector<std::string>& files) const
  {
    files.clear ();
    directory_iterator_c it (*this);
    directory_iterator_c end;
    while  (it != end)
      {
	files.push_back (it.name ());
	++it;
      }
  }
  // ---------------------------------------------------------------------
  void file_c::list (std::vector<file_c>& files) const
  {
    files.clear ();
    directory_iterator_c it (*this);
    directory_iterator_c end;
    while  (it != end)
      {
	files.push_back (*it);
	++it;
      }
  }
  // ---------------------------------------------------------------------
  void file_c::handle_last_error (const std::string& path)
  {
    handleLastErrorImpl (path);
  }
  // ---------------------------------------------------------------------
  const std::string& file_c::path () const
  {
    return getPathImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::operator ==  (const file_c& file) const
  {
    return getPathImpl () == file.getPathImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::operator !=  (const file_c& file) const
  {
    return getPathImpl () != file.getPathImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::operator <  (const file_c& file) const
  {
    return getPathImpl () < file.getPathImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::operator <=  (const file_c& file) const
  {
    return getPathImpl () <= file.getPathImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::operator >  (const file_c& file) const
  {
    return getPathImpl () > file.getPathImpl ();
  }
  // ---------------------------------------------------------------------
  bool file_c::operator >=  (const file_c& file) const
  {
    return getPathImpl () >= file.getPathImpl ();
  }
  // ---------------------------------------------------------------------
  void swap (file_c& f1, file_c& f2)
  {
    f1.swap (f2);
  }
} 
