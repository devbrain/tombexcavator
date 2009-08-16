#ifndef __BROKEN_COMPILER_H__
#define __BROKEN_COMPILER_H__

#if defined(_MSC_VER) && !defined(_MFC_VER)

#ifndef NOMINMAX
#define NOMINMAX
#endif

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif
 // replace missing template definitions in VC6
#ifdef __cplusplus 
#pragma warning(disable: 4786)
#pragma warning(disable: 4355)
#if defined(_MSC_VER) && (_MSC_VER < 1300)
namespace std
{
 template<typename T> const T& max(const T& l, const T& r) {return l > r ? l : r;}
 template<typename T> const T& min(const T& l, const T& r) {return l < r ? l : r;}
}
#ifndef snprintf
#define snprintf _snprintf
#endif

#endif
#endif
#endif

/* broken math constants */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef M_E
#define M_E 2.718281828459045235360
#endif


#endif
