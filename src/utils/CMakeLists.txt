MACRO(ADD_UTILITY target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE tombexcavator-formats thirdparty::mio)
    set_target_properties(${target} PROPERTIES CXX_STANDARD 17)
ENDMACRO()

ADD_UTILITY(load_pic load_pic.cc)
ADD_UTILITY(mz_explode mz_explode.cc)
ADD_UTILITY(iff iff.cc)

add_executable(ccmap ccmap.cc)
target_link_libraries(ccmap PRIVATE tombexcavator-formats tombexcavator-export tombexcavator-games-apogee tombexcavator-provider)
set_target_properties(ccmap PROPERTIES CXX_STANDARD 17)


