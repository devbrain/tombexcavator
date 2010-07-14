#ifndef __ABL_STRING_TEXT_ITERATOR_HPP__
#define __ABL_STRING_TEXT_ITERATOR_HPP__

#include <string>
#include "abl/abl_export.h" 



namespace abl 
{


  class text_encoding_c;
  /// An unidirectional iterator for iterating over characters in a string.
  /// The TextIterator uses a text_encoding_c object to
  /// work with multi-byte character encodings like UTF-8.
  /// Characters are reported in Unicode.
  ///
  /// Example: Count the number of UTF-8 characters in a string.
  ///
  ///     UTF8Encoding utf8Encoding;
  ///     std::string utf8String("....");
  ///     TextIterator it(utf8String, utf8Encoding);
  ///     TextIterator end(utf8String);
  ///     int n = 0;
  ///     while (it != end) { ++n; ++it; }
  ///
  /// NOTE: When an UTF-16 encoding is used, surrogate pairs will be
  /// reported as two separate characters, due to restrictions of
  /// the text_encoding_c class.

  class ABL_API text_iterator_c

  {
  public:
    text_iterator_c();
    /// Creates an uninitialized text_iterator_c.
		
    text_iterator_c(const std::string& str, const text_encoding_c& encoding);
    /// Creates a text_iterator_c for the given string.
    /// The encoding object must not be deleted as long as the iterator
    /// is in use.

    text_iterator_c(const std::string::const_iterator& begin, 
		    const std::string::const_iterator& end, const text_encoding_c& encoding);
    /// Creates a text_iterator_c for the given range.
    /// The encoding object must not be deleted as long as the iterator
    /// is in use.

    text_iterator_c(const std::string& str);
    /// Creates an end text_iterator_c for the given string.

    text_iterator_c(const std::string::const_iterator& end);
    /// Creates an end text_iterator_c.

    ~text_iterator_c();
    /// Destroys the text_iterator_c.
	
    text_iterator_c(const text_iterator_c& it);
    /// Copy constructor.
	
    text_iterator_c& operator = (const text_iterator_c& it);
    /// Assignment operator.
		
    void swap(text_iterator_c& it);
    /// Swaps the iterator with another one.
	
    int operator * () const;
    /// Returns the Unicode value of the current character.
    /// If there is no valid character at the current position,
    /// -1 is returned.
		
    text_iterator_c& operator ++ (); 
    /// Prefix increment operator.

    text_iterator_c operator ++ (int);		
    /// Postfix increment operator.

    bool operator == (const text_iterator_c& it) const;
    /// Compares two iterators for equality.
		
    bool operator != (const text_iterator_c& it) const;
    /// Compares two iterators for inequality.
		
  private:
    const text_encoding_c*      _pEncoding;
    std::string::const_iterator _it;
    std::string::const_iterator _end;
  };


  void swap (text_iterator_c& it1, text_iterator_c& it2);

} // namespace abl


#endif 
