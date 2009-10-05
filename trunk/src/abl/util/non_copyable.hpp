#ifndef __ABL_UTIL_NON_COPYABLE_HPP__
#define __ABL_UTIL_NON_COPYABLE_HPP__

#if !defined(INLINE_API)
#define INLINE_API
#endif

namespace util
{
  class INLINE_API non_copyable_c
  {
  protected:
    non_copyable_c () {}
    virtual ~non_copyable_c () {}
    
  private:
    // emphasize cc and assignment are private
    non_copyable_c(const non_copyable_c&);
    const non_copyable_c& operator=(const non_copyable_c&);
  };
}

#endif
