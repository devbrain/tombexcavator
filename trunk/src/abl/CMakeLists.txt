set (abl_sources 
  physfs/win32/dirent.c
  physfs/dir_reader.c
  unicode/unicode.c
)

set (abl_headers
  physfs/win32/dirent.h
  physfs/dir_reader.h
  unicode/unicode.h
  util/types.hpp 
  util/static_assert.hpp 
  util/non_copyable.hpp
)

add_library (abl SHARED ${provider_sources} ${provider_headers})
