set (libgff_sources 
	iff_reader.cpp
)

set (libgff_headers 
	gff_export.hpp
	iff.hpp
)

add_library (gff SHARED ${libgff_sources} ${libgff_headers})
target_link_libraries (gff abl)
set_target_properties (gff PROPERTIES COMPILE_FLAGS "-DBUILD_GFF ") 
