set (libwestwood_sources
  pak_file.cpp
)

set (libwestwood_headers
  pak_file.hpp
)

add_library (westwood SHARED ${libwestwood_sources} ${libwestwood_headers})
set_target_properties (westwood PROPERTIES COMPILE_FLAGS "-DBUILD_WESTWOOD ")
target_link_libraries (westwood provider vfs)

