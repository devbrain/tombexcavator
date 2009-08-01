set (sources 
  directory.cpp
  file.cpp
)

set (headers
  directory.hpp
  file.hpp
) 

add_library (provider_dune2 SHARED ${sources} ${headers})
target_link_libraries (provider_dune2 westwood provider vfs)
