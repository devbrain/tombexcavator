add_executable (glob_demo glob_demo.cpp)
target_link_libraries (glob_demo abl)


add_executable (extractor extractor.cpp)
target_link_libraries (extractor vfs abl)

add_executable (cps cps.cpp)