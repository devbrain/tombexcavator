# -----------------------------------------------------------------
# Unmystic - mystic towers tester
# -----------------------------------------------------------------

add_executable(unmystic main.cc mystic_towers.cc mystic_towers.hh)
target_link_libraries(unmystic PRIVATE te::archive vfs::vfs)
add_dependencies(unmystic mio_ext)

