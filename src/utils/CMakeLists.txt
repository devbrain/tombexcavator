MACRO(ADD_UTILITY target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tombexcavator-formats thirdparty::mio)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17)
ENDMACRO()

ADD_UTILITY(load_pic load_pic.cc)
ADD_UTILITY(mz_explode mz_explode.cc)

