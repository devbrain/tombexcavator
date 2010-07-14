#include "abl/fs/directory_iterator.hpp"

#if defined(_WIN32)
#include <windows.h>
#include <cstring>

#include "abl/string/unicode_converter.hpp"
 
namespace abl 
{
  class ABL_API directory_iterator_impl_c
  {
  public:
    directory_iterator_impl_c (const std::string& path);
    ~directory_iterator_impl_c ();
	
    void duplicate ();
    void release ();
	
    const std::string& get () const;
    const std::string& next ();
	
  private:
    HANDLE           _fh;
    WIN32_FIND_DATAW _fd;
    std::string      _current;
    int _rc;
  };
  // ----------------------------------------------------------------
  const std::string& directory_iterator_impl_c::get () const
  {
    return _current;
  }
  // ----------------------------------------------------------------
  void directory_iterator_impl_c::duplicate ()
  {
    ++_rc;
  }
  // ----------------------------------------------------------------
  void directory_iterator_impl_c::release ()
  {
    if  (--_rc == 0)
      delete this;
  }
  // ----------------------------------------------------------------
  directory_iterator_impl_c::directory_iterator_impl_c (const std::string& path)
    : _fh (INVALID_HANDLE_VALUE), 
      _rc (1)
  {
    path_c p (path);
    p.make_directory ();
    std::string findPath = p.to_string ();
    findPath.append ("*");
    std::wstring uFindPath;
    unicode_converter_c::to_utf16 (findPath, uFindPath);

    _fh = FindFirstFileW (uFindPath.c_str (), &_fd);
    if  (_fh == INVALID_HANDLE_VALUE)
      {
	if  (GetLastError () != ERROR_NO_MORE_FILES)
	  file_c::handle_last_error (path);
      }
    else
      {
	unicode_converter_c::to_utf8 (_fd.cFileName, _current);
	if  (_current == "." || _current == "..")	
	  next ();
      }
  }
  // ----------------------------------------------------------------

  directory_iterator_impl_c::~directory_iterator_impl_c ()
  {
    if  (_fh != INVALID_HANDLE_VALUE)
      FindClose (_fh);
  }
  // ----------------------------------------------------------------

  const std::string& directory_iterator_impl_c::next ()
  {
    do
      {
	_current.clear ();
	if  (FindNextFileW (_fh, &_fd) != 0)
	  {
	    unicode_converter_c::to_utf8 (_fd.cFileName, _current);
	  }
      }
    while  (_current == "." || _current == "..");
    return _current;
  }

 
} 
#else
#include <dirent.h>


namespace abl 
{


  class ABL_API directory_iterator_impl_c
  {
  public:
    directory_iterator_impl_c (const std::string& path);
    ~directory_iterator_impl_c ();
	
    void duplicate ();
    void release ();
	
    const std::string& get () const;
    const std::string& next ();
	
  private:
    DIR*        _pDir;
    std::string _current;
    int _rc;
  };
  // ----------------------------------------------------------------
  const std::string& directory_iterator_impl_c::get () const
  {
    return _current;
  }
  // ----------------------------------------------------------------
  void directory_iterator_impl_c::duplicate ()
  {
    ++_rc;
  }
  // ----------------------------------------------------------------
  void directory_iterator_impl_c::release ()
  {
    if  (--_rc == 0)
      delete this;
  }
  // ----------------------------------------------------------------
  directory_iterator_impl_c::directory_iterator_impl_c (const std::string& path)
    : _pDir (0), 
      _rc (1)
  {
    path_c p (path);
    p.make_file ();

    _pDir = opendir (p.to_string ().c_str ());
    if  (!_pDir) file_c::handle_last_error (path);

    next ();
  }
  // ----------------------------------------------------------------
  directory_iterator_impl_c::~directory_iterator_impl_c ()
  {
    if  (_pDir) closedir (_pDir);
  }
  // ----------------------------------------------------------------
  const std::string& directory_iterator_impl_c::next ()
  {
    do
      {
	struct dirent* pEntry = readdir (_pDir);
	if  (pEntry)
	  _current = pEntry->d_name;
	else
	  _current.clear ();
      }
    while  (_current == "." || _current == "..");
    return _current;
  }
 
} // namespace Poco 
#endif



namespace abl 
{
  directory_iterator_c::directory_iterator_c ()
    : m_pimpl (0)
  {
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c::directory_iterator_c (const std::string& path)
    : m_path (path), 
      m_pimpl (new directory_iterator_impl_c  (path))
  {
    m_path.make_directory ();
    m_path.set_file_name (m_pimpl->get ());
    m_file = m_path;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c::directory_iterator_c (const directory_iterator_c& iterator)
    : m_path (iterator.m_path), 
      m_pimpl (iterator.m_pimpl)
  {
    if  (m_pimpl) 
      {
	m_pimpl->duplicate ();
	m_file = m_path;
      }
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c::directory_iterator_c (const file_c& file)
    : m_path (file.path ()), 
      m_pimpl (new directory_iterator_impl_c (file.path ()))
  {
    m_path.make_directory ();
    m_path.set_file_name (m_pimpl->get ());
    m_file = m_path;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c::directory_iterator_c (const path_c& path)
    : m_path (path), 
      m_pimpl (new directory_iterator_impl_c (path.to_string ()))
  {
    m_path.make_directory ();
    m_path.set_file_name (m_pimpl->get ());
    m_file = m_path;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c::~directory_iterator_c ()
  {
    if  (m_pimpl) m_pimpl->release ();
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c& directory_iterator_c::operator =  (const directory_iterator_c& it)
  {
    if  (m_pimpl) m_pimpl->release ();
    m_pimpl = it.m_pimpl;
    if  (m_pimpl) 
      {
	m_pimpl->duplicate ();
	m_path = it.m_path;
	m_file = m_path;
      }
    return *this;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c& directory_iterator_c::operator =  (const file_c& file)
  {
    if  (m_pimpl) m_pimpl->release ();
    m_pimpl = new directory_iterator_impl_c (file.path ());
    m_path.parse_directory (file.path ());
    m_path.set_file_name (m_pimpl->get ());
    m_file = m_path;
    return *this;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c& directory_iterator_c::operator =  (const path_c& path)
  {
    if  (m_pimpl) m_pimpl->release ();
    m_pimpl = new directory_iterator_impl_c (path.to_string ());
    m_path = path;
    m_path.make_directory ();
    m_path.set_file_name (m_pimpl->get ());
    m_file = m_path;
    return *this;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c& directory_iterator_c::operator =  (const std::string& path)
  {
    if  (m_pimpl) m_pimpl->release ();
    m_pimpl = new directory_iterator_impl_c (path);
    m_path.parse_directory (path);
    m_path.set_file_name (m_pimpl->get ());
    m_file = m_path;
    return *this;
  }
  // -----------------------------------------------------------------------------------
  directory_iterator_c& directory_iterator_c::operator ++  ()
  {
    if  (m_pimpl)
      {
	m_path.set_file_name (m_pimpl->next ());
	m_file = m_path;
      }
    return *this;
  }
  // -----------------------------------------------------------------------------------
  const std::string& directory_iterator_c::name () const
  {
    return m_path.get_file_name ();
  }
  // -----------------------------------------------------------------------------------
  const path_c& directory_iterator_c::path () const
  {
    return m_path;
  }
  // -----------------------------------------------------------------------------------
  const file_c& directory_iterator_c::operator *  () const
  {
    return m_file;
  }
  // -----------------------------------------------------------------------------------
  file_c& directory_iterator_c::operator *  ()
  {
    return m_file;
  }
  // -----------------------------------------------------------------------------------
  const file_c* directory_iterator_c::operator ->  () const
  {
    return &m_file;
  }
  // -----------------------------------------------------------------------------------
  file_c* directory_iterator_c::operator ->  ()
  {
    return &m_file;
  }
  // -----------------------------------------------------------------------------------
  bool directory_iterator_c::operator ==  (const directory_iterator_c& iterator) const
  {
    return name () == iterator.name ();
  }
  // -----------------------------------------------------------------------------------
  bool directory_iterator_c::operator !=  (const directory_iterator_c& iterator) const
  {
    return name () != iterator.name ();
  }
} 
