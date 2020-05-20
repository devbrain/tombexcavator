add_library(stargunner SHARED stargunner.cc stargunner.hh stargunner_fs.cc stargunner_fs.hh)
target_link_libraries(stargunner PRIVATE te::archive vfs::vfs)
add_dependencies(stargunner mio_ext)
# -----------------------------------------------------------------
# Ungunner - stargunner tester
# -----------------------------------------------------------------

add_executable(ungunner main.cc stargunner.cc stargunner.hh)
target_link_libraries(ungunner PRIVATE te::archive vfs::vfs)
add_dependencies(ungunner mio_ext)

