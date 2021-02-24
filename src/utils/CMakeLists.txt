MACRO(ADD_UTILITY target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tombexcavator-formats thirdparty::mio)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17)
ENDMACRO()

ADD_UTILITY(load_pic load_pic.cc)
ADD_UTILITY(mz_explode mz_explode.cc)
# ADD_UTILITY(ega_reader ega_reader.cc)


add_executable(te-cli te-cli.cc)
target_link_libraries(te-cli PRIVATE tombexcavator-provider)
set_target_properties(te-cli PROPERTIES CXX_STANDARD 17)


