set (provider_sources 
  resource.cpp
  archive_reader.cpp
)

set (provide_headers
  resource.hpp
  archive_reader.hpp
)

add_library (provider STATIC ${provider_sources} ${provider_headers})
