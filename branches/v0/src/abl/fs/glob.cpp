#include "abl/fs/glob.hpp"
#include "abl/fs/path.hpp"
#include "abl/exception.hpp"
#include "abl/fs/directory_iterator.hpp"
#include "abl/fs/file.hpp"
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
    poco_assert (!m_pattern.empty());
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
    glob (path_c (path_c::expand(path_pattern), path_c::PATH_GUESS), files, options);
  }
  // -----------------------------------------------------------------------------
  void glob_c::glob (const char* path_pattern, std::set<std::string>& files, int options)
  {
    glob (path_c (path_c::expand(path_pattern), path_c::PATH_GUESS), files, options);
  }
  // -----------------------------------------------------------------------------
  void glob_c::glob (const path_c& path_pattern, std::set <std::string>& files, int options)
  {
    path_c pattern (path_pattern);
    pattern.make_directory(); // to simplify pattern handling later on
    path_c base     (pattern);
    path_c abs_base (base);
    abs_base.make_absolute ();
    while (base.depth () > 0 && base [base.depth () - 1] != "..") 
      {
	base.pop_directory ();
	abs_base.pop_directory ();
      }
    if (path_pattern.is_directory ()) 
      {
	options |= GLOB_DIRS_ONLY;
      }
    collect (pattern, abs_base, base, path_pattern [base.depth ()], files, options);		
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

  void glob_c::collect (const path_c& pathPattern, const path_c& base, 
			const path_c& current    , const std::string& pattern, 
			std::set <std::string>& files, int options)
  {
    try
      {
	std::string pp = pathPattern.to_string();
	std::string basep = base.to_string();
	std::string curp  = current.to_string();
	glob_c g(pattern, options);
	directory_iterator_c it(base);
	directory_iterator_c end;
	while (it != end)
	  {
	    const std::string& name = it.name();
	    if (g.match(name))
	      {
		path_c p(current);
		if (p.depth() < pathPattern.depth() - 1)
		  {
		    p.push_directory(name);
		    collect(pathPattern, it.path(), p, pathPattern[p.depth()], files, options);
		  }
		else
		  {
		    p.set_file_name(name);
		    if (is_directory(p, (options & GLOB_FOLLOW_SYMLINKS) != 0))
		      {
			p.make_directory();
			files.insert(p.to_string());
		      }
		    else if (!(options & GLOB_DIRS_ONLY))
		      {
			files.insert(p.to_string());
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
  bool glob_c::is_directory (const path_c& path, bool follow_symlink)
  {
    file_c f(path);
    if (f.is_directory())
      {
	return true;
      }
    else if (follow_symlink && f.is_link())
      {
	try
	  {
	    // Test if link resolves to a directory.
	    directory_iterator_c it(f);
	    return true;
	  }
	catch (exception_c&)
	  {
	  }
      }
    return false;
  }


} // namespace Poco
