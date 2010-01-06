set (provider_sources 
  resource.cpp
  archive_reader.cpp
  simple_dir.cpp
)

set (provide_headers
  resource.hpp
  archive_reader.hpp
  simple_dir.hpp
)

add_library (provider SHARED ${provider_sources} ${provider_headers})
set_target_properties (provider PROPERTIES COMPILE_FLAGS "-DBUILD_LIBPROVIDER ") 
target_link_libraries (provider vfs abl)