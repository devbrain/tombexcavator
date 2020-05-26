add_library(mystic_towers SHARED mystic_towers_fs.cc mystic_towers.cc mystic_towers.hh)
target_link_libraries(mystic_towers PRIVATE provider)
# -----------------------------------------------------------------
# Unmystic - mystic towers tester
# -----------------------------------------------------------------
add_executable(unmystic main.cc mystic_towers.cc mystic_towers.hh)
target_link_libraries(unmystic PRIVATE te::archive)
add_dependencies(unmystic mio_ext)

