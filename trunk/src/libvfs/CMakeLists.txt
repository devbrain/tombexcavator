set (libvfs_sources 
  fs.cpp 
  inode_defs.cpp 
  fs_object.cpp
  fs_file.cpp
  fs_dir.cpp
)

set (libvfs_headers 
  fs.hpp 
  inode_defs.hpp 
  fs_object.hpp
  fs_file.hpp
  fs_dir.hpp
  util/types.hpp 
  util/static_assert.hpp 
  util/non_copyable.hpp
)

add_library (vfs STATIC ${libvfs_sources} ${libfvs_headers})
