/**
 * @file   tokenizer.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 14:28:02 2009
 * 
 * @brief  Definition of the tokenizer_c class.
 */

#ifndef __ABL_STRING_TOKENIZER_HPP__
#define __ABL_STRING_TOKENIZER_HPP__


#include "abl/abl_export.h"

#include <vector>
#include <string>
#include <cstddef>


namespace abl
{
  /**
   * A simple tokenizer that splits a string into
   * tokens, which are separated by separator characters.
   * An iterator is used to iterate over all tokens.
   */
  class ABL_API tokenizer_c
	
  {
  public:
    enum options_t
      {
	TOK_IGNORE_EMPTY = 1, /**< ignore empty tokens */
	TOK_TRIM         = 2  /**< remove leading and trailing whitespace from tokens */
      };
	
    typedef std::vector <std::string>::const_iterator iterator_t;
    /**
     * Splits the given string into tokens. The tokens are expected to be
     * separated by one of the separator characters given in separators.
     * Additionally, options can be specified:
     *   * TOK_IGNORE_EMPTY: empty tokens are ignored
     *   * TOK_TRIM: trailing and leading whitespace is removed from tokens.
     * An empty token at the end of str is always ignored. For example,
     * a tokenizer_c with the following arguments:
     *     tokenizer_c(",ab,cd,", ",");
     * will produce three tokens, "", "ab" and "cd".
     */
    tokenizer_c (const std::string& str, const std::string& separators, int options = 0);
	

    ~tokenizer_c ();

	
    iterator_t begin () const;
    iterator_t end () const;
    /**
     * Returns the index'th token.
     * Throws a RangeException if the index is out of range.
     */
    const std::string& operator [] (std::size_t index) const;
    /**
     * Returns the number of tokens.
     */
    std::size_t count () const;
		

  private:
    tokenizer_c (const tokenizer_c&);
    tokenizer_c& operator = (const tokenizer_c&);
	
    std::vector <std::string> m_tokens;
  };
} 


#endif 
