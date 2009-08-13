set (provider_sources 
  resource.cpp
  archive_reader.cpp
  physfs/win32/dirent.c
  physfs/unicode.c
  physfs/dir_reader.c
)

set (provide_headers
  resource.hpp
  archive_reader.hpp
  physfs/win32/dirent.h
  physfs/unicode.h
  physfs/dir_reader.h
)

add_library (provider STATIC ${provider_sources} ${provider_headers})
