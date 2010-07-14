#include "abl/string/regular_expression.hpp"
#include "abl/exception.hpp"
#include <sstream>
#include <assert.h>
#include "abl/unicode/pcre.h"

#define poco_assert assert

namespace abl 
{
  const int regular_expression_c::OVEC_SIZE = 64;
  // ------------------------------------------------------------------------------------------------
  regular_expression_c::regular_expression_c (const std::string& pattern, int options, bool study)
    : m_pcre (0), 
      m_extra (0)
  {
    const char* error;
    int offs;
    m_pcre = pcre_compile (pattern.c_str (), options, &error, &offs, 0);
    if  (!m_pcre)
      {
	std::ostringstream msg;
	msg << error << "  (at offset " << offs << ")";
	throw regular_expression_exception_c (msg.str ());
      }
    if  (study)
      m_extra = pcre_study (m_pcre, 0, &error);
  }
  // ------------------------------------------------------------------------------------------------
  regular_expression_c::~regular_expression_c ()
  {
    if  (m_pcre)  pcre_free (m_pcre);
    if  (m_extra) pcre_free (m_extra);
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::match (const std::string& subject, std::string::size_type offset, 
				   match_s& mtch, int options) const
  {
    poco_assert  (offset <= subject.length ());

    int ovec[OVEC_SIZE];
    int rc = pcre_exec (m_pcre, m_extra, subject.c_str (), 
			int (subject.size ()), int (offset), options & 0xFFFF, ovec, OVEC_SIZE);
    if  (rc == PCRE_ERROR_NOMATCH)
      {
	mtch.offset = std::string::npos;
	mtch.length = 0;
	return 0;
      }
    else if  (rc == PCRE_ERROR_BADOPTION)
      {
	throw regular_expression_exception_c ("bad option");
      }
    else if  (rc == 0)
      {
	throw regular_expression_exception_c ("too many captured substrings");
      }
    else if  (rc < 0)
      {
	std::ostringstream msg;
	msg << "PCRE error " << rc;
	throw regular_expression_exception_c (msg.str ());
      }
    mtch.offset = ovec[0] < 0 ? std::string::npos : ovec[0];
    mtch.length = ovec[1] - mtch.offset;
    return rc;
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::match (const std::string& subject, std::string::size_type offset, 
				   match_vec_t& matches, int options) const
  {
    poco_assert  (offset <= subject.length ());

    matches.clear ();

    int ovec[OVEC_SIZE];
    int rc = pcre_exec (m_pcre, m_extra, subject.c_str (), int (subject.size ()), 
			int (offset), options & 0xFFFF, ovec, OVEC_SIZE);
    if  (rc == PCRE_ERROR_NOMATCH)
      {
	return 0;
      }
    else if  (rc == PCRE_ERROR_BADOPTION)
      {
	throw regular_expression_exception_c ("bad option");
      }
    else if  (rc == 0)
      {
	throw regular_expression_exception_c ("too many captured substrings");
      }
    else if  (rc < 0)
      {
	std::ostringstream msg;
	msg << "PCRE error " << rc;
	throw regular_expression_exception_c (msg.str ());
      }
    matches.reserve (rc);
    for  (int i = 0; i < rc; ++i)
      {
	match_s m;
	m.offset = ovec[i*2] < 0 ? std::string::npos : ovec[i*2] ;
	m.length = ovec[i*2 + 1] - m.offset;
	matches.push_back (m);
      }
    return rc;
  }
  // ------------------------------------------------------------------------------------------------
  bool regular_expression_c::match (const std::string& subject, std::string::size_type offset) const
  {
    match_s mtch;
    match (subject, offset, mtch, RE_ANCHORED | RE_NOTEMPTY);
    return mtch.offset == offset && mtch.length == subject.length () - offset;
  }
  // ------------------------------------------------------------------------------------------------
  bool regular_expression_c::match (const std::string& subject, std::string::size_type offset, int options) const
  {
    match_s mtch;
    match (subject, offset, mtch, options);
    return mtch.offset == offset && mtch.length == subject.length () - offset;
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::extract (const std::string& subject, std::string& str, int options) const
  {
    match_s mtch;
    int rc = match (subject, 0, mtch, options);
    if  (mtch.offset != std::string::npos)
      str.assign (subject, mtch.offset, mtch.length);
    else
      str.clear ();
    return rc;
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::extract (const std::string& subject, std::string::size_type offset, 
				     std::string& str, int options) const
  {
    match_s mtch;
    int rc = match (subject, offset, mtch, options);
    if  (mtch.offset != std::string::npos)
      str.assign (subject, mtch.offset, mtch.length);
    else
      str.clear ();
    return rc;
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::split (const std::string& subject, std::string::size_type offset, 
				   std::vector<std::string>& strings, int options) const
  {
    match_vec_t matches;
    strings.clear ();
    int rc = match (subject, offset, matches, options);
    strings.reserve (matches.size ());
    for  (match_vec_t::const_iterator it = matches.begin (); it != matches.end (); ++it)
      {
	if  (it->offset != std::string::npos)
	  strings.push_back (subject.substr (it->offset, it->length));
	else
	  strings.push_back (std::string ());
      }
    return rc;
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::subst (std::string& subject, std::string::size_type offset, 
				   const std::string& replacement, int options) const
  {
    if  (options & RE_GLOBAL)
      {
	int rc = 0;
	std::string::size_type pos = _subst_one (subject, offset, replacement, options);
	while  (pos != std::string::npos)
	  {
	    ++rc;
	    pos = _subst_one (subject, pos, replacement, options);
	  }
	return rc;
      }
    else
      {
	return _subst_one (subject, offset, replacement, options) != std::string::npos ? 1 : 0;
      }
  }
  // ------------------------------------------------------------------------------------------------
  std::string::size_type regular_expression_c::_subst_one (std::string& subject, std::string::size_type offset, 
							   const std::string& replacement, int options) const
  {
    if  (offset >= subject.length ()) return std::string::npos;

    int ovec[OVEC_SIZE];
    int rc = pcre_exec (m_pcre, m_extra, subject.c_str (), 
			int (subject.size ()), int (offset), 
			options & 0xFFFF, ovec, OVEC_SIZE);
    if  (rc == PCRE_ERROR_NOMATCH)
      {
	return std::string::npos;
      }
    else if  (rc == PCRE_ERROR_BADOPTION)
      {
	throw regular_expression_exception_c ("bad option");
      }
    else if  (rc == 0)
      {
	throw regular_expression_exception_c ("too many captured substrings");
      }
    else if  (rc < 0)
      {
	std::ostringstream msg;
	msg << "PCRE error " << rc;
	throw regular_expression_exception_c (msg.str ());
      }
    std::string result;
    std::string::size_type len = subject.length ();
    std::string::size_type pos = 0;
    std::string::size_type rp = std::string::npos;
    while  (pos < len)
      {
	if  (ovec[0] == pos)
	  {
	    std::string::const_iterator it  = replacement.begin ();
	    std::string::const_iterator end = replacement.end ();
	    while  (it != end)
	      {
		if  (*it == '$' && ! (options & RE_NO_VARS))
		  {
		    ++it;
		    if  (it != end)
		      {
			char d = *it;
			if  (d >= '0' && d <= '9')
			  {
			    int c = d - '0';
			    if  (c < rc)
			      {
				int o = ovec[c*2];
				int l = ovec[c*2 + 1] - o;
				result.append (subject, o, l);
			      }
			  }
			else
			  {
			    result += '$';
			    result += d;
			  }
			++it;
		      }
		    else result += '$';
		  }
		else result += *it++;
	      }
	    pos = ovec[1];
	    rp = result.length ();
	  }
	else result += subject[pos++];
      }
    subject = result;
    return rp;
  }
  // ------------------------------------------------------------------------------------------------
  bool regular_expression_c::match (const std::string& subject, const std::string& pattern, int options)
  {
    int ctorOptions = options &  (RE_CASELESS | RE_MULTILINE      | RE_DOTALL | RE_EXTENDED | 
				  RE_ANCHORED | RE_DOLLAR_ENDONLY | RE_EXTRA  | RE_UNGREEDY | 
				  RE_UTF8     | RE_NO_AUTO_CAPTURE);
    int mtchOptions = options &  (RE_ANCHORED        | RE_NOTBOL       | RE_NOTEOL | RE_NOTEMPTY | 
				  RE_NO_AUTO_CAPTURE | RE_NO_UTF8_CHECK);
    regular_expression_c re (pattern, ctorOptions, false);
    return re.match (subject, 0, mtchOptions);
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::match (const std::string& subject, match_s& mtch, int options) const
  {
    return match (subject, 0, mtch, options);
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::split (const std::string& subject, 
				   std::vector<std::string>& strings, int options) const
  {
    return split (subject, 0, strings, options);
  }
  // ------------------------------------------------------------------------------------------------
  int regular_expression_c::subst (std::string& subject, const std::string& replacement, int options) const
  {
    return subst (subject, 0, replacement, options);
  }
  // ------------------------------------------------------------------------------------------------
  bool regular_expression_c::operator ==  (const std::string& subject) const
  {
    return match (subject);
  }
  // ------------------------------------------------------------------------------------------------
  bool regular_expression_c::operator !=  (const std::string& subject) const
  {
    return !match (subject);
  }
  // ------------------------------------------------------------------------------------------------
}

