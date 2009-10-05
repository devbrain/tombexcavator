set (abl_sources 
  dl/dl.c
  mutex/mutex.c
  physfs/win32/dirent.c
  physfs/dir_reader.c
  unicode/unicode.c
)

set (abl_headers
  dl/dl.h
  mutex/mutex.h
  physfs/win32/dirent.h
  physfs/dir_reader.h
  unicode/unicode.h
  util/types.hpp 
  util/static_assert.hpp 
  util/non_copyable.hpp
)

add_library (abl SHARED ${abl_sources} ${abl_headers})
set_target_properties (abl PROPERTIES COMPILE_FLAGS "-DBUILD_ABL ")  
