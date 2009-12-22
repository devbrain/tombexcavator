#include "abl/fs/glob.hpp"
#include "abl/fs/path.hpp"
#include "abl/exception.hpp"
#include "abl/fs/directory_iterator.hpp"
#include "abl/file.hpp"
#include "abl/string/encoding/utf8.hpp"
#include "abl/unicode/unicode.h"

#include <assert.h>
#define poco_assert assert

namespace abl 
{
  glob_c::glob_c (const std::string& pattern, int options)
    : m_pattern(pattern),
      m_options(options)
  {
    poco_assert (!_pattern.empty());
  }
  // -----------------------------------------------------------------------------
  glob_c::~glob_c ()
  {
  }
  // -----------------------------------------------------------------------------
  bool glob_c::match(const std::string& subject)
  {
    utf8_encoding_c utf8;
    text_iterator_c itp  (m_pattern, utf8);
    text_iterator_c endp (m_pattern);
    text_iterator_c its  (subject, utf8);
    text_iterator_c ends (subject);
	
    if ((m_options & GLOB_DOT_SPECIAL) && its != ends && *its == '.' && (*itp == '?' || *itp == '*'))
      {
	return false;
      }
    return match (itp, endp, its, ends);
  }
  // -----------------------------------------------------------------------------
  void glob_c::glob (const std::string& path_pattern, std::set <std::string>& files, int options)
  {
    glob (Path (Path::expand(path_pattern), Path::PATH_GUESS), files, options);
  }
  // -----------------------------------------------------------------------------
  void glob_c::glob (const char* pathPattern, std::set<std::string>& files, int options)
  {
    glob (Path (Path::expand(path_pattern), Path::PATH_GUESS), files, options);
  }
  // -----------------------------------------------------------------------------
  void glob_c::glob (const Path& path_pattern, std::set <std::string>& files, int options)
  {
    Path pattern (path_pattern);
    pattern.makeDirectory(); // to simplify pattern handling later on
    Path base     (pattern);
    Path abs_base (base);
    abs_base.makeAbsolute ();
    while (base.depth () > 0 && base [base.depth () - 1] != "..") 
      {
	base.popDirectory ();
	absBase.popDirectory ();
      }
    if (path_pattern.is_directory ()) 
      {
	options |= GLOB_DIRS_ONLY;
      }
    collect (pattern, absBase, base, path_pattern [base.depth ()], files, options);		
  }
  // -----------------------------------------------------------------------------
  bool glob_c::match (text_iterator_c& itp, const text_iterator_c& endp, 
		      text_iterator_c& its, const text_iterator_c& ends)
  {
    while (itp != endp)
      {
	if (its == ends)
	  {
	    while (itp != endp && *itp == '*') 
	      {
		++itp;
	      }
	    break;
	  }
	switch (*itp)
	  {
	  case '?':
	    ++itp; 
	    ++its;
	    break;
	  case '*':
	    if (++itp != endp)
	      {
		while (its != ends && !match_after_asterisk (itp, endp, its, ends)) 
		  {
		    ++its;
		  }
		return (its != ends);
	      }
	    return true;
	  case '[':
	    if (++itp != endp) 
	      {
		bool invert = *itp == '!';
		if (invert) ++itp;
		if (itp != endp)
		  {
		    bool mtch = match_set (itp, endp, *its++);
		    if ((invert && mtch) || (!invert && !mtch)) return false;
		    break;
		  }
	      }
	    throw syntax_exception_c ("bad range syntax in glob pattern");
	  case '\\':
	    if (++itp == endp) 
	      {
		throw syntax_exception_c ("backslash must be followed by character in glob pattern");
	      }
	    // fallthrough
	  default:
	    if (m_options & GLOB_CASELESS)
	      {
		if (unicode_to_lower (*itp) != unicode_to_lower (*its)) 
		  {
		    return false;
		  }
	      }
	    else
	      {
		if (*itp != *its) 
		  {
		    return false;
		  }
	      }
	    ++itp; 
	    ++its;
	  }
      }
    return (itp == endp && its == ends);
  }
  // -----------------------------------------------------------------------------
  bool glob_c::match_after_asterisk (text_iterator_c itp, const text_iterator_c& endp, 
				     text_iterator_c its, const text_iterator_c& ends)
  {
    return match (itp, endp, its, ends);
  }
  // -----------------------------------------------------------------------------
  bool glob_c::match_set (text_iterator_c& itp, const text_iterator_c& endp, int c)
  {
    if (m_options & GLOB_CASELESS)
      {
	c = unicode_to_lower (c);
      }

    while (itp != endp)
      {
	switch (*itp)
	  {
	  case ']':
	    ++itp; 
	    return false;
	  case '\\':
	    if (++itp == endp) 
	      {
		throw syntax_exception_c ("backslash must be followed by character in glob pattern");
	      }
	  }
	int first = *itp;
	int last  = first;
	if (++itp != endp && *itp == '-')
	  {
	    if (++itp != endp)
	      {
		last = *itp++;
	      }
	    else
	      throw syntax_exception_c ("bad range syntax in glob pattern");
	  }
	if (m_options & GLOB_CASELESS)
	  {
	    first = unicode_to_lower (first);
	    last  = unicode_to_lower (last);
	  }
	if (first <= c && c <= last)
	  {
	    while (itp != endp)
	      {
		switch (*itp)
		  {
		  case ']':
		    ++itp;
		    return true;
		  case '\\':
		    if (++itp == endp) 
		      {
			break;
		      }
		  default:
		    ++itp;
		  }
	      }
	    throw syntax_exception_c ("range must be terminated by closing bracket in glob pattern");
	  }
      }
    return false;
  }

  // ----------------------------------------------------------------------------

  void glob_c::collect (const Path& pathPattern, const Path& base, 
			const Path& current    , const std::string& pattern, 
			std::set <std::string>& files, int options)
  {
    try
      {
	std::string pp = pathPattern.toString();
	std::string basep = base.toString();
	std::string curp  = current.toString();
	Glob g(pattern, options);
	DirectoryIterator it(base);
	DirectoryIterator end;
	while (it != end)
	  {
	    const std::string& name = it.name();
	    if (g.match(name))
	      {
		Path p(current);
		if (p.depth() < pathPattern.depth() - 1)
		  {
		    p.pushDirectory(name);
		    collect(pathPattern, it.path(), p, pathPattern[p.depth()], files, options);
		  }
		else
		  {
		    p.setFileName(name);
		    if (isDirectory(p, (options & GLOB_FOLLOW_SYMLINKS) != 0))
		      {
			p.makeDirectory();
			files.insert(p.toString());
		      }
		    else if (!(options & GLOB_DIRS_ONLY))
		      {
			files.insert(p.toString());
		      }
		  }
	      }
	    ++it;
	  }
      }
    catch (exception_c&)
      {
      }
  }
  // ---------------------------------------------------------------------------------
  bool glob_c::is_directory (const Path& path, bool follow_symlink)
  {
    File f(path);
    if (f.is_directory())
      {
	return true;
      }
    else if (follow_symlink && f.is_link())
      {
	try
	  {
	    // Test if link resolves to a directory.
	    DirectoryIterator it(f);
	    return true;
	  }
	catch (exception_c&)
	  {
	  }
      }
    return false;
  }


} // namespace Poco
