set (sources 
  directory.cpp
  file.cpp
  dune2_fs.cpp
)

set (headers
  directory.hpp
  file.hpp
  dune2_fs.hpp
) 

add_library (provider_dune2 SHARED ${sources} ${headers})
target_link_libraries (provider_dune2 westwood provider vfs abl)
set_target_properties (provider_dune2 PROPERTIES COMPILE_FLAGS
"-DBUILD_DUNE2 ")