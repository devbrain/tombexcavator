add_library(stargunner SHARED stargunner.cc stargunner.hh stargunner_fs.cc stargunner_fs.hh)
target_link_libraries(stargunner te::archive)

add_executable(ungunner main.cc stargunner.cc stargunner.hh)
target_link_libraries(ungunner te::archive)

