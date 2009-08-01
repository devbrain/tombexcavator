#ifndef __LIBVFS_UTIL_NON_COPYABLE_HPP__
#define __LIBVFS_UTIL_NON_COPYABLE_HPP__

namespace util
{

  class non_copyable_c
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
