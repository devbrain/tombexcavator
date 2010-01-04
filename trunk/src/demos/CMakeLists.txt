add_executable (glob_demo glob_demo.cpp)
target_link_libraries (glob_demo abl)


add_executable (dune2_dump dune2_dump.cpp)
target_link_libraries (dune2_dump vfs abl)
