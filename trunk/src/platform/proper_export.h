#ifndef __PLATFORM_PROPER_EXPORT_H__
#define __PLATFORM_PROPER_EXPORT_H__

# if defined(__BEOS__) || defined(__HAIKU__)
#  if defined(__GNUC__)
#   define DECLSPEC_EXPORT __declspec(dllexport)
#  else
#   define DECLSPEC_EXPORT __declspec(export)
#  endif
# elif defined(__WIN32__)
#  ifdef __BORLANDC__
#    define DECLSPEC_EXPORT 
#    define DECLSPEC_IMPORT	__declspec(dllimport)
#  else
#   define DECLSPEC_EXPORT	__declspec(dllexport)
#  endif
# elif defined(__OS2__)
#  ifdef __WATCOMC__
#    define DECLSPEC_EXPORT	__declspec(dllexport)
#    define DECLSPEC_IMPORT
#  elif defined (__GNUC__) && __GNUC__ < 4
#    define DECLSPEC_EXPORT    __declspec(dllexport)
#    define DECLSPEC_IMPORT
#  else
#   define DECLSPEC_EXPORT
#   define DECLSPEC_IMPORT
#  endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define DECLSPEC_EXPORT	__attribute__ ((visibility("default")))
#   define DECLSPEC_IMPORT
#  else
#   define DECLSPEC_EXPORT
#   define DECLSPEC_IMPORT
#  endif
# endif

# if defined(__WIN32__) && !defined(__GNUC__)
#  define CALLSPEC __cdecl
# elif defined(__OS2__)
#  if defined (__GNUC__) && __GNUC__ < 4
#   define CALLSPEC _cdecl
#  else
#   define CALLSPEC _System
#  endif
# else
#  define CALLSPEC
# endif

#ifdef __SYMBIAN32__ 
#ifndef EKA2 
#undef DECLSPEC_IMPORT
#define DECLSPEC_IMPORT
#undef DECLSPEC_EXPORT
#define DECLSPEC_EXPORT
#elif !defined(__WINS__)
#undef DECLSPEC_EXPORT
#define DECLSPEC_EXPORT __declspec(dllexport)
#endif /* !EKA2 */
#endif /* __SYMBIAN32__ */



#if !defined(DECLSPEC_EXPORT)
#error "DECLSPEC_EXPORT is undefined"
#endif

#if !defined(DECLSPEC_IMPORT)
#error "DECLSPEC_EXPORT is undefined"
#endif

#if !defined(CALLSPEC)
#error "CALLSPEC is undefined"
#endif


#endif
