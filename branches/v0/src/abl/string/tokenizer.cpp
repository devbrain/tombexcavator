#include "abl/string/tokenizer.hpp"
#include "abl/exception.hpp"
#include <cctype>


namespace abl 
{
  tokenizer_c::tokenizer_c (const std::string& str, const std::string& separators, int options)
  {
    std::string::const_iterator it1 = str.begin();
    std::string::const_iterator it2;
    std::string::const_iterator it3;
    std::string::const_iterator end = str.end();
	
    while (it1 != end)
      {
	if (options & TOK_TRIM)
	  {
	    while (it1 != end && std::isspace (*it1)) 
	      {
		++it1;
	      }
	  }
	it2 = it1;
	while (it2 != end && separators.find (*it2) == std::string::npos) 
	  {
	    ++it2;
	  }
	it3 = it2;
	if (it3 != it1 && (options & TOK_TRIM))
	  {
	    --it3;
	    while (it3 != it1 && std::isspace (*it3)) 
	      {
		--it3;
	      }
	    if (!std::isspace(*it3)) 
	      {
		++it3;
	      }
	  }
	if (options & TOK_IGNORE_EMPTY)
	  {
	    if (it3 != it1)
	      {
		m_tokens.push_back(std::string (it1, it3));
	      }
	  }
	else
	  {
	    m_tokens.push_back(std::string (it1, it3));
	  }
	it1 = it2;
	if (it1 != end) 
	  {
	    ++it1;
	  }
      }
  }
  // --------------------------------------------------------------------------------------
  tokenizer_c::~tokenizer_c ()
  {
  }
  // --------------------------------------------------------------------------------------
  tokenizer_c::iterator_t tokenizer_c::begin() const
  {
    return m_tokens.begin();
  }
  // --------------------------------------------------------------------------------------
  tokenizer_c::iterator_t tokenizer_c::end() const
  {
    return m_tokens.end();
  }
  // --------------------------------------------------------------------------------------
  const std::string& tokenizer_c::operator [] (std::size_t index) const
  {
    if (index >= m_tokens.size()) throw range_exception_c ();
    return m_tokens[index];
  }
  // --------------------------------------------------------------------------------------
  std::size_t tokenizer_c::count() const
  {
    return m_tokens.size();
  }

} // namespace abl
