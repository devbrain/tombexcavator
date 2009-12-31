set (abl_sources 
  platform/dl/dl.c
  platform/mutex/mutex.c
  platform/mutex/rwlock.c
  platform/physfs/win32/dirent.c
  platform/physfs/dir_reader.c
  unicode/unicode_converter.c
  unicode/unicode.c
  mt/mutex.cpp
  mt/rw_lock.cpp
  mt/null_mutex.cpp


  unicode/pcre_chartables.c    unicode/pcre_compile.c       unicode/pcre_exec.c
  unicode/pcre_fullinfo.c      unicode/pcre_globals.c       unicode/pcre_maketables.c
  unicode/pcre_newline.c       unicode/pcre_ord2utf8.c      unicode/pcre_study.c
  unicode/pcre_tables.c        unicode/pcre_try_flipped.c   unicode/pcre_ucd.c
  unicode/pcre_valid_utf8.c    unicode/pcre_xclass.c

  exception.cpp
  environment.cpp
  number_formatter.cpp
  number_parser.cpp
  string/encoding/ascii.cpp
  string/encoding/latin1.cpp
  string/encoding/latin9.cpp
  string/encoding/win1252.cpp
  string/encoding/utf8.cpp
  string/encoding/utf16.cpp

  string/text_encoding.cpp
  string/text_iterator.cpp
  string/tokenizer.cpp
  string/unicode_converter.cpp
  string/text_converter.cpp
  string/regular_expression.cpp

  date_time/time_stamp.cpp
  date_time/time_span.cpp
  date_time/time_zone.cpp
  date_time/date_time.cpp
  date_time/local_date_time.cpp
  date_time/date_time_format.cpp
  date_time/date_time_parser.cpp
  date_time/date_time_formatter.cpp

  fs/directory_iterator.cpp
  fs/file_impl.cpp
  fs/file.cpp
  fs/path.cpp
  fs/glob.cpp
)

set (abl_headers
  platform/dl/dl.h
  platform/mutex/mutex.h
  platform/mutex/rwlock.h
  platform/physfs/win32/dirent.h
  platform/physfs/dir_reader.h
  unicode/unicode_converter.h
  unicode/unicode.h
  unicode/unicode_types.h
  unicode/pcre_config.h
  unicode/pcre_internal.h
  types/types.hpp 
  util/static_assert.hpp 
  util/non_copyable.hpp
  mt/mutex.hpp
  mt/rw_lock.hpp
  mt/null_mutex.hpp
  mt/atomic_counter.hpp
  mt/guard.hpp
  unicode/pcre.h unicode/pcre_config.h unicode/pcre_internal.h unicode/ucp.h
  throw_spec.hpp
  exception.hpp
  number_formatter.hpp
  number_parser.hpp
  environment.hpp

  string/encoding/ascii.hpp
  string/encoding/latin1.hpp
  string/encoding/latin9.hpp
  string/encoding/win1252.hpp
  string/encoding/utf8.hpp
  string/encoding/utf16.hpp

  string/text_encoding.hpp
  string/text_iterator.hpp
  string/tokenizer.hpp
  string/text_converter.hpp
  string/unicode_converter.hpp
  string/regular_expression.hpp
  byte_order.hpp

  date_time/time_stamp.hpp
  date_time/time_span.hpp
  date_time/time_zone.hpp
  date_time/date_time.hpp
  date_time/local_date_time.hpp
  date_time/date_time_format.hpp
  date_time/date_time_parser.hpp
  date_time/date_time_formatter.hpp

  fs/directory_iterator.hpp
  fs/file_impl.hpp
  fs/file.hpp
  fs/path.hpp
  fs/glob.hpp
)

add_library (abl SHARED ${abl_sources} ${abl_headers})
set_target_properties (abl PROPERTIES COMPILE_FLAGS "-DBUILD_ABL ")  
