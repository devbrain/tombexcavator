#ifndef __LIBFVS_UTILS_STATIC_ASSERT_HPP__
#define __LIBFVS_UTILS_STATIC_ASSERT_HPP__


#if !defined (PASTE)
#define PASTE2( x, y) x##y
#define PASTE( x, y)  PASTE2( x, y)
#endif /* PASTE */

#if !defined (PASTE_LINE)
#define PASTE_LINE( x)    PASTE( x, __LINE__)
#endif /* PASTE_LINE */

#if!defined( PASTE_COUNTER)
#if (_MSC_VER >= 1300)      /* __COUNTER__ introduced in VS 7 (VS.NET 2002) */
    #define PASTE_COUNTER( x) PASTE( x, __COUNTER__)   /* __COUNTER__ is a an _MSC_VER >= 1300 non-Ansi extension */
#else
    #define PASTE_COUNTER( x) PASTE( x, __LINE__)      /* since there's no __COUNTER__ use __LINE__ as a more or less reasonable substitute */
#endif
#endif /* PASTE_COUNTER */



namespace util 
{
  template<bool b> struct StaticAssert_failed;
  template<>       struct StaticAssert_failed <true> 
  { 
    enum {val = 1 }; 
  };
  template<int x>  struct StaticAssert_test { };
}

#define CPP_ASSERT( expr) typedef ::util::StaticAssert_test< sizeof( ::util::StaticAssert_failed< (bool) (expr) >) >  PASTE_COUNTER( StaticAssertType_)

#define STATIC_ASSERT( expr)    CPP_ASSERT( expr)

#endif

