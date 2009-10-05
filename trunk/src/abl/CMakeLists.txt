set (abl_sources 
  platform/dl/dl.c
  platform/mutex/mutex.c
  platform/physfs/win32/dirent.c
  platform/physfs/dir_reader.c
  unicode/unicode.c
  mutex.cpp
  null_mutex.cpp
)

set (abl_headers
  platform/dl/dl.h
  platform/mutex/mutex.h
  platform/physfs/win32/dirent.h
  platform/physfs/dir_reader.h
  unicode/unicode.h
  util/types.hpp 
  util/static_assert.hpp 
  util/non_copyable.hpp
  mutex.hpp
  null_mutex.hpp
  throw_spec.hpp
)

add_library (abl SHARED ${abl_sources} ${abl_headers})
set_target_properties (abl PROPERTIES COMPILE_FLAGS "-DBUILD_ABL ")  
