set (abl_sources 
  platform/dl/dl.c
  platform/mutex/mutex.c
  platform/physfs/win32/dirent.c
  platform/physfs/dir_reader.c
  unicode/unicode_converter.c
  unicode/unicode.c
  mt/mutex.cpp
  mt/null_mutex.cpp


  unicode/pcre_chartables.c    unicode/pcre_compile.c       unicode/pcre_exec.c
  unicode/pcre_fullinfo.c      unicode/pcre_globals.c       unicode/pcre_maketables.c
  unicode/pcre_newline.c       unicode/pcre_ord2utf8.c      unicode/pcre_study.c
  unicode/pcre_tables.c        unicode/pcre_try_flipped.c   unicode/pcre_ucd.c
  unicode/pcre_valid_utf8.c    unicode/pcre_xclass.c
)

set (abl_headers
  platform/dl/dl.h
  platform/mutex/mutex.h
  platform/physfs/win32/dirent.h
  platform/physfs/dir_reader.h
  unicode/unicode_converter.h
  unicode/unicode.h
  unicode/unicode_types.h
  unicode/pcre_config.h
  unicode/pcre_internal.h
  util/types.hpp 
  util/static_assert.hpp 
  util/non_copyable.hpp
  mt/mutex.hpp
  mt/null_mutex.hpp
  mt/atomic_counter.hpp
  mt/guard.hpp
  unicode/pcre.h unicode/pcre_config.h unicode/pcre_internal.h unicode/ucp.h
  throw_spec.hpp
)

add_library (abl SHARED ${abl_sources} ${abl_headers})
set_target_properties (abl PROPERTIES COMPILE_FLAGS "-DBUILD_ABL ")  
