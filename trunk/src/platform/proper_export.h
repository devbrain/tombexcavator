#ifndef __PLATFORM_PROPER_EXPORT_H__
#define __PLATFORM_PROPER_EXPORT_H__

# if defined(__BEOS__) || defined(__HAIKU__)
#  if defined(__GNUC__)
#   define TE_DECLSPEC_EXPORT __declspec(dllexport)
#  else
#   define TE_DECLSPEC_EXPORT __declspec(export)
#  endif
# elif defined(__WIN32__) || defined(WIN32) 
#  ifdef __BORLANDC__
#    define TE_DECLSPEC_EXPORT 
#    define TE_DECLSPEC_IMPORT	__declspec(dllimport)
#  else
#   define TE_DECLSPEC_EXPORT	__declspec(dllexport)
#   define TE_DECLSPEC_IMPORT	__declspec(dllimport)
#  endif
# elif defined(__OS2__)
#  ifdef __WATCOMC__
#    define TE_DECLSPEC_EXPORT	__declspec(dllexport)
#    define TE_DECLSPEC_IMPORT
#  elif defined (__GNUC__) && __GNUC__ < 4
#    define TE_DECLSPEC_EXPORT    __declspec(dllexport)
#    define TE_DECLSPEC_IMPORT
#  else
#   define TE_DECLSPEC_EXPORT
#   define TE_DECLSPEC_IMPORT
#  endif
# else
#  if defined(__GNUC__) && __GNUC__ >= 4
#   define TE_DECLSPEC_EXPORT	__attribute__ ((visibility("default")))
#   define TE_DECLSPEC_IMPORT
#  else
#   define TE_DECLSPEC_EXPORT
#   define TE_DECLSPEC_IMPORT
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
#undef TE_DECLSPEC_IMPORT
#define TE_DECLSPEC_IMPORT
#undef TE_DECLSPEC_EXPORT
#define TE_DECLSPEC_EXPORT
#elif !defined(__WINS__)
#undef TE_DECLSPEC_EXPORT
#define TE_DECLSPEC_EXPORT __declspec(dllexport)
#endif /* !EKA2 */
#endif /* __SYMBIAN32__ */



#if !defined(TE_DECLSPEC_EXPORT)
#error "TE_DECLSPEC_EXPORT is undefined"
#endif

#if !defined(TE_DECLSPEC_IMPORT)
#error "TE_DECLSPEC_EXPORT is undefined"
#endif

#if !defined(CALLSPEC)
#error "CALLSPEC is undefined"
#endif


#endif
