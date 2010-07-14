set (libvfs_sources 
  fs.cpp 
  inode_defs.cpp 
  fs_object.cpp
  fs_file.cpp
  fs_dir.cpp
  
  visitor.cpp
  extractor_visitor.cpp
  dump_visitor.cpp
  print_visitor.cpp

  for_each.cpp
)

set (libvfs_headers 
  fs.hpp 
  inode_defs.hpp 
  fs_object.hpp
  fs_file.hpp
  fs_dir.hpp
  vfs_export.hpp

  visitor.hpp
  extractor_visitor.hpp
  dump_visitor.hpp
  print_visitor.hpp

  for_each.hpp
)

add_library (vfs SHARED ${libvfs_sources} ${libfvs_headers})
target_link_libraries (vfs abl)
set_target_properties (vfs PROPERTIES COMPILE_FLAGS "-DBUILD_VFS ") 
