#include "abl/fs/path.hpp"
#include "abl/fs/file.hpp"
#include "abl/exception.hpp"
#include "abl/string/tokenizer.hpp"
#if defined(_WIN32)
#include <windows.h>
#include <cstring>
#include "abl/string/unicode_converter.hpp"
#include "abl/buffer.hpp"
#include "abl/environment.hpp"
#else
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <pwd.h>
#include <cctype>
#include <climits> 
#endif
#include <algorithm>

#include <assert.h>
#define poco_assert assert
#define poco_check_ptr assert
inline
static void poco_bugcheck () {assert (0);} 

#if !defined (NAME_MAX)
#  if defined (MAXNAMLEN)
#    define NAME_MAX MAXNAMLEN
#  elif defined (MAXNAMELEN)
#    define NAME_MAX MAXNAMELEN
#  elif defined (FILENAME_MAX)
#    define NAME_MAX FILENAME_MAX
#  elif defined (_MAX_FNAME)
#    define NAME_MAX _MAX_FNAME
#  else /* _MAX_FNAME */
#    define NAME_MAX 256
#  endif /* MAXNAMLEN */
#endif /* !NAME_MAX */

#if !defined (MAXNAMELEN)
#  define MAXNAMELEN NAME_MAX
#endif /* MAXNAMELEN */

#if !defined (HOST_NAME_MAX)
#  define HOST_NAME_MAX 256
#endif /* !HOST_NAME_MAX */

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

// Leaving this for backward compatibility, but PATH_MAX should always be
// used directly.
#if !defined (MAX_PATH_LEN)
#  define MAX_PATH_LEN  PATH_MAX
#endif /* !MAXPATHLEN */ 

namespace abl 
{
  class path_impl_c
  {
  public:
    static std::string current ();
    static std::string home ();
    static std::string temp ();
    static std::string null ();
    static std::string expand (const std::string& path);
    static void list_roots (std::vector<std::string>& roots);
  }; 
#ifndef _WIN32
  std::string path_impl_c::current ()
  {
    std::string path;
    char cwd[PATH_MAX];
    if  (getcwd (cwd, sizeof (cwd)))
      path = cwd;
    else
      throw system_exception_c ("cannot get current directory");
    std::string::size_type n = path.size ();
    if  (n > 0 && path[n - 1] != '/') path.append ("/");
    return path;
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::home ()
  {
    std::string path;
    struct passwd* pwd = getpwuid (getuid ());
    if  (pwd)
      path = pwd->pw_dir;
    else
      {
	pwd = getpwuid (geteuid ());
	if  (pwd)
	  path = pwd->pw_dir;
	else
	  {
	    const char* h = getenv ("HOME");
	    if (h)
	      path = h;
	    else
	      path = "";
	  }
      }
    std::string::size_type n = path.size ();
    if  (n > 0 && path[n - 1] != '/') path.append ("/");
    return path;
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::temp ()
  {
    std::string path;
    char* tmp = getenv ("TMPDIR");
    if  (tmp)
      {
	path = tmp;
	std::string::size_type n = path.size ();
	if  (n > 0 && path[n - 1] != '/') path.append ("/");
      }
    else
      {
	path = "/tmp/";
      }
    return path;
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::null ()
  {
    return "/dev/null";
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::expand (const std::string& path)
  {
    std::string result;
    std::string::const_iterator it  = path.begin ();
    std::string::const_iterator end = path.end ();
    if  (it != end && *it == '~')
      {
	++it;
	if  (it != end && *it == '/')
	  {
	    result += home (); ++it;
	  }
	else result += '~';
      }
    while  (it != end)
      {
	if  (*it == '$')
	  {
	    std::string var;
	    ++it;
	    if  (it != end && *it == '{')
	      {
		++it;
		while  (it != end && *it != '}') var += *it++;
		if  (it != end) ++it;
	      }
	    else
	      {
		while  (it != end &&  (std::isalnum (*it) || *it == '_')) var += *it++;
	      }
	    char* val = getenv (var.c_str ());
	    if  (val) result += val;
	  }
	else result += *it++;
      }
    return result;
  }
  // ----------------------------------------------------------------------------
  void path_impl_c::list_roots (std::vector<std::string>& roots)
  {
    roots.clear ();
    roots.push_back ("/");
  }
#else
  std::string path_impl_c::current ()
  {
    std::string result;
    DWORD len = GetCurrentDirectoryW (0, NULL);
    if  (len > 0)
      {
	buffer_c<wchar_t> buffer (len);
	DWORD n = GetCurrentDirectoryW (len, buffer.begin ());
	if  (n > 0 && n <= len)
	  {
	    unicode_converter_c::to_utf8 (buffer.begin (), result);
	    if  (result[n - 1] != '\\')
	      result.append ("\\");
	    return result;
	  }
      }
    throw system_exception_c ("Cannot get current directory");
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::home ()
  {
    std::string result = environment_c::get ("HOMEDRIVE");
    result.append (environment_c::get ("HOMEPATH"));
    std::string::size_type n = result.size ();
    if  (n > 0 && result[n - 1] != '\\')
      result.append ("\\");
    return result;
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::temp ()
  {
    buffer_c<wchar_t> buffer (MAX_PATH_LEN);
    DWORD n = GetTempPathW (static_cast<DWORD> (buffer.size ()), buffer.begin ());
    if  (n > 0)
      {
	std::string result;
	unicode_converter_c::to_utf8 (buffer.begin (), result);
	if  (result[n - 1] != '\\')
	  result.append ("\\");
	return result;
      }
    throw system_exception_c ("Cannot get current directory");
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::null ()
  {
    return "NUL:";
  }
  // ----------------------------------------------------------------------------
  std::string path_impl_c::expand (const std::string& path)
  {
    std::wstring upath;
    unicode_converter_c::to_utf16 (path, upath);
    buffer_c<wchar_t> buffer (MAX_PATH_LEN);
    DWORD n = ExpandEnvironmentStringsW (upath.c_str (), buffer.begin (), static_cast<DWORD> (buffer.size ()));
    if  (n > 0 && n < buffer.size () - 1)
      {
	buffer[n + 1] = 0;
	std::string result;
	unicode_converter_c::to_utf8 (buffer.begin (), result);
	return result;
      }
    else return path;
  }
  // ----------------------------------------------------------------------------
  void path_impl_c::list_roots (std::vector<std::string>& roots)
  {
    roots.clear ();
    wchar_t buffer[128];
    DWORD n = GetLogicalDriveStringsW (sizeof (buffer)/sizeof (wchar_t) - 1, buffer);
    wchar_t* it  = buffer;
    wchar_t* end = buffer +  (n > sizeof (buffer) ? sizeof (buffer) : n);
    while  (it < end)
      {
	std::wstring udev;
	while  (it < end && *it) udev += *it++;
	std::string dev;
	unicode_converter_c::to_utf8 (udev, dev);
	roots.push_back (dev);
	++it;
      }
  } 
#endif
  // =============================================================================
  path_c::path_c ()
    : m_absolute (false)
  {
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (bool absolute)
    : m_absolute (absolute)
  {
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (const std::string& path)
  {
    assign (path);
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (const std::string& path, style_t style)
  {
    assign (path, style);
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (const char* path)
  {
    poco_check_ptr (path);
    assign (path);
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (const char* path, style_t style)
  {
    poco_check_ptr (path);
    assign (path, style);
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (const path_c& path)
    : m_node (path.m_node), 
      m_device (path.m_device),
      m_name (path.m_name),
      m_dirs (path.m_dirs),
      m_absolute (path.m_absolute)
  {	
  }
  // -----------------------------------------------------------------------------

  path_c::path_c (const path_c& parent, const std::string& fileName)
    : m_node (parent.m_node), 
      m_device (parent.m_device),
      m_name (parent.m_name),
      m_dirs (parent.m_dirs),
      m_absolute (parent.m_absolute)
  {	
    make_directory ();
    m_name = fileName;
  }
  // -----------------------------------------------------------------------------

  path_c::path_c (const path_c& parent, const char* fileName)
    : m_node (parent.m_node), 
      m_device (parent.m_device),
      m_name (parent.m_name),
      m_dirs (parent.m_dirs),
      m_absolute (parent.m_absolute)
  {	
    make_directory ();
    m_name = fileName;
  }
  // -----------------------------------------------------------------------------
  path_c::path_c (const path_c& parent, const path_c& relative)
    : m_node (parent.m_node), 
      m_device (parent.m_device),
      m_name (parent.m_name),
      m_dirs (parent.m_dirs),
      m_absolute (parent.m_absolute)
  {	
    resolve (relative);
  }
  // -----------------------------------------------------------------------------
  path_c::~path_c ()
  {
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::operator =  (const path_c& path)
  {
    return assign (path);
  }
  // -----------------------------------------------------------------------------
	
  path_c& path_c::operator =  (const std::string& path)
  {
    return assign (path);
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::operator =  (const char* path)
  {
    poco_check_ptr (path);
    return assign (path);
  }
  // -----------------------------------------------------------------------------
  void path_c::swap (path_c& path)
  {
    std::swap (m_node, path.m_node);
    std::swap (m_device, path.m_device);
    std::swap (m_name, path.m_name);
    std::swap (m_dirs, path.m_dirs);
    std::swap (m_absolute, path.m_absolute);
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::assign (const path_c& path)
  {
    if  (&path != this)
      {
	m_node     = path.m_node;
	m_device   = path.m_device;
	m_name     = path.m_name;
	m_dirs     = path.m_dirs;
	m_absolute = path.m_absolute;
      }
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::assign (const std::string& path)
  {
#if defined (_WIN32)
    _parse_windows (path);
#else
    _parse_unix (path);
#endif
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::assign (const std::string& path, style_t style)
  {
    switch  (style)
      {
      case PATH_UNIX:
	_parse_unix (path);
	break;
      case PATH_WINDOWS:
	_parse_windows (path);
	break;
      case PATH_NATIVE:
	assign (path);
	break;
      case PATH_GUESS:
	_parse_guess (path);
	break;
      default:
	poco_bugcheck ();
      }
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::assign (const char* path)
  {
    return assign (std::string (path));
  }
  // -----------------------------------------------------------------------------

  std::string path_c::to_string () const
  {
#if !defined (_WIN32)
    return _build_unix ();
#else
    return _build_windows ();
#endif
  }
  // -----------------------------------------------------------------------------
  std::string path_c::to_string (style_t style) const
  {
    switch  (style)
      {
      case PATH_UNIX:
	return _build_unix ();
      case PATH_WINDOWS:
	return _build_windows ();
      case PATH_NATIVE:
      case PATH_GUESS:
	return to_string ();
      default:
	poco_bugcheck ();
      }
    return std::string ();
  }
  // -----------------------------------------------------------------------------
  bool path_c::try_parse (const std::string& path)
  {
    try
      {
	path_c p;
	p.parse (path);
	assign (p);
	return true;
      }
    catch  (...)
      {
	return false;
      }
  }
  // -----------------------------------------------------------------------------
  bool path_c::try_parse (const std::string& path, style_t style)
  {
    try
      {
	path_c p;
	p.parse (path, style);
	assign (p);
	return true;
      }
    catch  (...)
      {
	return false;
      }
  }

  // -----------------------------------------------------------------------------
  path_c& path_c::parse_directory (const std::string& path)
  {
    assign (path);
    return make_directory ();
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::parse_directory (const std::string& path, style_t style)
  {
    assign (path, style);
    return make_directory ();
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::make_directory ()
  {
    push_directory (m_name);
    m_name.clear ();
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::make_file ()
  {
    if  (!m_dirs.empty () && m_name.empty ())
      {
	m_name = m_dirs.back ();
	m_dirs.pop_back ();
      }
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::make_absolute ()
  {
    return make_absolute (current ());
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::make_absolute (const path_c& base)
  {
    if  (!m_absolute)
      {
	path_c tmp = base;
	tmp.make_directory ();
	for  (string_vec_t::const_iterator it = m_dirs.begin (); it != m_dirs.end (); ++it)
	  {
	    tmp.push_directory (*it);
	  }
	m_node     = tmp.m_node;
	m_device   = tmp.m_device;
	m_dirs     = tmp.m_dirs;
	m_absolute = base.m_absolute;
      }
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c path_c::absolute () const
  {
    path_c result (*this);
    if  (!result.m_absolute)
      {
	result.make_absolute ();
      }
    return result;
  }
  // -----------------------------------------------------------------------------
  path_c path_c::absolute (const path_c& base) const
  {
    path_c result (*this);
    if  (!result.m_absolute)
      {
	result.make_absolute (base);
      }
    return result;
  }
  // -----------------------------------------------------------------------------
  path_c path_c::parent () const
  {
    path_c p (*this);
    return p.make_parent ();
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::make_parent ()
  {
    if  (m_name.empty ())
      {
	if  (m_dirs.empty ())
	  {
	    if  (!m_absolute)
	      m_dirs.push_back ("..");
	  }
	else
	  {
	    if  (m_dirs.back () == "..")
	      m_dirs.push_back ("..");
	    else
	      m_dirs.pop_back ();
	  }
      }
    else
      {
	m_name.clear ();
      }
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::append (const path_c& path)
  {
    make_directory ();
    m_dirs.insert (m_dirs.end (), path.m_dirs.begin (), path.m_dirs.end ());
    m_name = path.m_name;
    return *this;
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::resolve (const path_c& path)
  {
    if  (path.is_absolute ())
      {
	assign (path);
      }
    else
      {
	for  (int i = 0; i < path.depth (); ++i)
	  push_directory (path[i]);
	m_name = path.m_name;
      }
    return *this;
  }
  // -----------------------------------------------------------------------------
  void path_c::set_node (const std::string& node)
  {
    m_node     = node;
    m_absolute = m_absolute || !node.empty ();
  }
  // -----------------------------------------------------------------------------
  void path_c::set_device (const std::string& device)
  {
    m_device   = device;
    m_absolute = m_absolute || !device.empty ();
  }
  // -----------------------------------------------------------------------------
  const std::string& path_c::directory (int n) const
  {
    poco_assert  (0 <= n && (size_t)n <= m_dirs.size ());
	
    if  ((size_t)n < m_dirs.size ())
      return m_dirs[n];
    else
      return m_name;	
  }
  // -----------------------------------------------------------------------------
  const std::string& path_c::operator []  (int n) const
  {
    poco_assert  (0 <= n && (size_t)n <= m_dirs.size ());
	
    if  ((size_t)n < m_dirs.size ())
      return m_dirs[n];
    else
      return m_name;	
  }
  // -----------------------------------------------------------------------------
  void path_c::push_directory (const std::string& dir)
  {
    if  (!dir.empty () && dir != ".")
      {
	if  (dir == "..")
	  {
	    if  (!m_dirs.empty () && m_dirs.back () != "..")
	      m_dirs.pop_back ();
	    else if  (!m_absolute)
	      m_dirs.push_back (dir);
	  }
	else m_dirs.push_back (dir);
      }
  }
  // -----------------------------------------------------------------------------
  void path_c::pop_directory ()
  {
    poco_assert  (!m_dirs.empty ());
	
    m_dirs.pop_back ();
  }
  // -----------------------------------------------------------------------------
  void path_c::set_file_name (const std::string& name)
  {
    m_name = name;
  }
  // -----------------------------------------------------------------------------
  void path_c::set_base_name (const std::string& name)
  {
    std::string ext = get_extension ();
    m_name = name;
    if  (!ext.empty ())
      {
	m_name.append (".");
	m_name.append (ext);
      }
  }
  // -----------------------------------------------------------------------------
  std::string path_c::get_base_name () const
  {
    std::string::size_type pos = m_name.rfind ('.');
    if  (pos != std::string::npos)
      return m_name.substr (0, pos);
    else
      return m_name;
  }
  // -----------------------------------------------------------------------------
  void path_c::set_extension (const std::string& extension)
  {
    m_name = get_base_name ();
    if  (!extension.empty ())
      {
	m_name.append (".");
	m_name.append (extension);
      }
  }
  // -----------------------------------------------------------------------------
  std::string path_c::get_extension () const
  {
    std::string::size_type pos = m_name.rfind ('.');
    if  (pos != std::string::npos)
      return m_name.substr (pos + 1);
    else
      return std::string ();
  }
  // -----------------------------------------------------------------------------
  void path_c::clear ()
  {
    m_node.clear ();
    m_device.clear ();
    m_name.clear ();
    m_dirs.clear ();
    m_absolute = false;
  }
  // -----------------------------------------------------------------------------
  std::string path_c::current ()
  {
    return path_impl_c::current ();
  }
  // -----------------------------------------------------------------------------
  std::string path_c::home ()
  {
    return path_impl_c::home ();
  }
  // -----------------------------------------------------------------------------
  std::string path_c::temp ()
  {
    return path_impl_c::temp ();
  }
  // -----------------------------------------------------------------------------
  std::string path_c::null ()
  {
    return path_impl_c::null ();
  }
  // -----------------------------------------------------------------------------
  std::string path_c::expand (const std::string& path)
  {
    return path_impl_c::expand (path);
  }
  // -----------------------------------------------------------------------------
  void path_c::list_roots (std::vector<std::string>& roots)
  {
    path_impl_c::list_roots (roots);
  }
  // -----------------------------------------------------------------------------
  bool path_c::find (string_vec_t::const_iterator it, string_vec_t::const_iterator end, 
		    const std::string& name, path_c& path)
  {
    while (it != end)
      {
	path_c p (*it);
	p.make_directory ();
	p.resolve (path_c (name));
	file_c f (p);
	if  (f.exists ())
	  {
	    path = p;
	    return true;
	  }
	++it;
      }
    return false;
  }
  // -----------------------------------------------------------------------------
  bool path_c::find (const std::string& pathList, const std::string& name, path_c& path)
  {
    tokenizer_c st (pathList, 
		    std::string (1, path_separator ()), 
		    tokenizer_c::TOK_IGNORE_EMPTY + tokenizer_c::TOK_TRIM);
    return find (st.begin (), st.end (), name, path);
  }
  // -----------------------------------------------------------------------------
  void path_c::_parse_unix (const std::string& path)
  {
    clear ();

    std::string::const_iterator it  = path.begin ();
    std::string::const_iterator end = path.end ();

    if  (it != end)
      {
	if  (*it == '/') 
	  {
	    m_absolute = true; ++it;
	  }
	else if  (*it == '~')
	  {
	    ++it;
	    if  (it == end || *it == '/')
	      {
		path_c cwd (home ());
		m_dirs = cwd.m_dirs;
		m_absolute = true;
	      }
	    else --it;
	  }

	while  (it != end)
	  {
	    std::string name;
	    while  (it != end && *it != '/') name += *it++;
	    if  (it != end)
	      {
		if  (m_dirs.empty ())
		  {
		    if  (!name.empty () && * (name.rbegin ()) == ':')
		      m_device.assign (name, 0, name.length () - 1);
		    else
		      push_directory (name);
		  }
		else push_directory (name);
	      }
	    else m_name = name;
	    if  (it != end) ++it;
	  }
      }
  }
  // -----------------------------------------------------------------------------
  void path_c::_parse_windows (const std::string& path)
  {
    clear ();

    std::string::const_iterator it  = path.begin ();
    std::string::const_iterator end = path.end ();

    if  (it != end)
      {
	if  (*it == '\\' || *it == '/') { m_absolute = true; ++it; }
	if  (m_absolute && it != end &&  (*it == '\\' || *it == '/')) // UNC
	  {
	    ++it;
	    while  (it != end && *it != '\\' && *it != '/') m_node += *it++;
	    if  (it != end) ++it;
	  }
	else if  (it != end)
	  {
	    char d = *it++;
	    if  (it != end && *it == ':') // drive letter
	      {
		if  (m_absolute || ! ( (d >= 'a' && d <= 'z') ||  (d >= 'A' && d <= 'Z'))) 
		  throw path_syntax_exception_c (path);
		m_absolute = true;
		m_device += d;
		++it;
		if  (it == end ||  (*it != '\\' && *it != '/')) 
		  throw path_syntax_exception_c (path);
		++it;
	      }
	    else --it;
	  }
	while  (it != end)
	  {
	    std::string name;
	    while  (it != end && *it != '\\' && *it != '/') name += *it++;
	    if  (it != end)
	      push_directory (name);
	    else
	      m_name = name;
	    if  (it != end) ++it;
	  }
      }
    if  (!m_node.empty () && m_dirs.empty () && !m_name.empty ())
      make_directory ();
  }
  // -----------------------------------------------------------------------------
  void path_c::_parse_guess (const std::string& path)
  {
    bool hasBackslash   = false;
    bool hasSlash       = false;
    bool hasOpenBracket = false;
    bool hasClosBracket = false;
    bool isWindows      = path.length () > 2 && path[1] == ':' &&  (path[2] == '/' || path[2] == '\\');
    std::string::const_iterator end    = path.end ();
    std::string::const_iterator semiIt = end;
    if  (!isWindows)
      {
	for  (std::string::const_iterator it = path.begin (); it != end; ++it)
	  {
	    switch  (*it)
	      {
	      case '\\': hasBackslash = true; break;
	      case '/':  hasSlash = true; break;
	      case '[':  hasOpenBracket = true;
	      case ']':  hasClosBracket = hasOpenBracket; 
	      case ';':  semiIt = it; break;
	      }
	  }
      }
    if  (hasBackslash || isWindows)
      {
	_parse_windows (path);
      }
    else if  (hasSlash)
      {
	_parse_unix (path);
      }
    else
      {
	_parse_unix (path);
      }
  }
  // -----------------------------------------------------------------------------
  std::string path_c::_build_unix () const
  {
    std::string result;
    if  (!m_device.empty ())
      {
	result.append ("/");
	result.append (m_device);
	result.append (":/");
      }
    else if  (m_absolute)
      {
	result.append ("/");
      }
    for  (string_vec_t::const_iterator it = m_dirs.begin (); it != m_dirs.end (); ++it)
      {
	result.append (*it);
	result.append ("/");
      }
    result.append (m_name);
    return result;
  }
  // -----------------------------------------------------------------------------
  std::string path_c::_build_windows () const
  {
    std::string result;
    if  (!m_node.empty ())
      {
	result.append ("\\\\");
	result.append (m_node);
	result.append ("\\");
      }
    else if  (!m_device.empty ())
      {
	result.append (m_device);
	result.append (":\\");
      }
    else if  (m_absolute)
      {
	result.append ("\\");
      }
    for  (string_vec_t::const_iterator it = m_dirs.begin (); it != m_dirs.end (); ++it)
      {
	result.append (*it);
	result.append ("\\");
      }
    result.append (m_name);
    return result;
  }
  // -----------------------------------------------------------------------------
  std::string path_c::transcode (const std::string& path)
  {
#if defined (_WIN32)
    std::wstring unipath_c;
    unicode_converter_c::to_utf16 (path, unipath_c);
    DWORD len = WideCharToMultiByte (CP_ACP, 
				     WC_NO_BEST_FIT_CHARS, 
				     unipath_c.c_str (), static_cast<int> (unipath_c.length ()), 
				     NULL, 
				     0, 
				     NULL, 
				     NULL);
    if  (len > 0)
      {
	buffer_c<char> buffer (len);
	DWORD rc = WideCharToMultiByte (CP_ACP, 
					WC_NO_BEST_FIT_CHARS, 
					unipath_c.c_str (), 
					static_cast<int> (unipath_c.length ()), 
					buffer.begin (), 
					static_cast<int> (buffer.size ()), 
					NULL, 
					NULL);
	if  (rc)
	  {
	    return std::string (buffer.begin (), buffer.size ());
	  }
      }
#endif
    return path;
  }
  // -------------------------------------------------------------------------------
  bool path_c::is_absolute () const
  {
    return m_absolute;
  }
  // -----------------------------------------------------------------------------
  bool path_c::is_relative () const
  {
    return !m_absolute;
  }
  // -----------------------------------------------------------------------------
  bool path_c::is_directory () const
  {
    return m_name.empty ();
  }
  // -----------------------------------------------------------------------------
  bool path_c::is_file () const
  {
    return !m_name.empty ();
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::parse (const std::string& path)
  {
    return assign (path);
  }
  // -----------------------------------------------------------------------------
  path_c& path_c::parse (const std::string& path, style_t style)
  {
    return assign (path, style);
  }
  // -----------------------------------------------------------------------------
  const std::string& path_c::get_node () const
  {
    return m_node;
  }
  // -----------------------------------------------------------------------------
  const std::string& path_c::get_device () const
  {
    return m_device;
  }
  // -----------------------------------------------------------------------------
  const std::string& path_c::get_file_name () const
  {
    return m_name;
  }
  // -----------------------------------------------------------------------------
  int path_c::depth () const
  {
    return int (m_dirs.size ());
  }
  // -----------------------------------------------------------------------------
  path_c path_c::for_directory (const std::string& path)
  {
    path_c p;
    return p.parse_directory (path);
  }
  // -----------------------------------------------------------------------------
  path_c path_c::for_directory (const std::string& path, style_t style)
  {
    path_c p;
    return p.parse_directory (path, style);
  }
  // -----------------------------------------------------------------------------
  char path_c::separator ()
  {
#if defined (_WIN32)
    return '\\';
#else
    return '/';
#endif
  }
  // -----------------------------------------------------------------------------

  char path_c::path_separator ()
  {
#if defined (_WIN32)
    return ';';
#else
    return ':';
#endif
  }
  // -----------------------------------------------------------------------------
  void swap (path_c& p1, path_c& p2)
  {
    p1.swap (p2);
  }
  // -----------------------------------------------------------------------------
} // namespace abl

