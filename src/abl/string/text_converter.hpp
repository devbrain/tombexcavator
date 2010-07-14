/**
 * @file   text_converter.hpp
 * @author  <igorgu@il.ibm.com>
 * @date   Tue Dec 22 15:03:10 2009
 * 
 * @brief  Definition of the text_converter class.
 * 
 */

#ifndef __ABL_STRING_TEXT_CONVERTER_HPP__
#define __ABL_STRING_TEXT_CONVERTER_HPP__


#include "abl/abl_export.h"
#include "abl/unicode/unicode_types.h"
#include <string>

namespace abl 
{
  class text_encoding_c;
  /**
   * A text_converter_c converts strings from one encoding
   * into another.
   */
  class ABL_API text_converter_c
  {
  public:
    
    typedef ucs4_t (*transform_t)(ucs4_t); /*< transform_t function for convert. */
    /**
     * Creates the text_converter_c. The encoding objects must not be deleted while the
     * text_converter_c is in use.
     */
    text_converter_c (const text_encoding_c& inEncoding, const text_encoding_c& outEncoding, int defaultChar = '?');
    
    
    ~text_converter_c ();
    /**
     * Converts the source string from inEncoding to outEncoding
     * and appends the result to destination. Every character is
     * passed to the transform function.
     * If a character cannot be represented in outEncoding, defaultChar
     * is used instead.
     * Returns the number of encoding errors (invalid byte sequences
     * in source).
     */
    int convert (const std::string& source, std::string& destination, transform_t trans);
    /**
     * Converts the source buffer from inEncoding to outEncoding
     * and appends the result to destination. Every character is
     * passed to the transform function.
     * If a character cannot be represented in outEncoding, defaultChar
     * is used instead.
     * Returns the number of encoding errors (invalid byte sequences
     * in source).
     */
    int convert (const void* source, int length, std::string& destination, transform_t trans);
    /**
     * Converts the source string from inEncoding to outEncoding
     * and appends the result to destination.
     * If a character cannot be represented in outEncoding, defaultChar
     * is used instead.
     * Returns the number of encoding errors (invalid byte sequences
     * in source).
     */
    int convert (const std::string& source, std::string& destination);

    /**
     * Converts the source buffer from inEncoding to outEncoding
     * and appends the result to destination.
     * If a character cannot be represented in outEncoding, defaultChar
     * is used instead.
     * Returns the number of encoding errors (invalid byte sequences
     * in source).
     */
    int convert (const void* source, int length, std::string& destination);
		
  private:
    text_converter_c ();
    text_converter_c (const text_converter_c&);
    text_converter_c& operator = (const text_converter_c&);
  private:
    const text_encoding_c& m_in_encoding;
    const text_encoding_c& m_out_encoding;
    int                    m_default_char;
  };


} 


#endif 
