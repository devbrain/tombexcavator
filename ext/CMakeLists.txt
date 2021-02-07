set (GLOBAL_OUTPUT_PATH ${CMAKE_BINARY_DIR}/ext)

file(MAKE_DIRECTORY ${GLOBAL_OUTPUT_PATH}/include)
file(MAKE_DIRECTORY ${GLOBAL_OUTPUT_PATH}/lib)

macro(IMPORT_THIRDPARTY EXT NAME LIB)
    ExternalProject_Get_Property(${EXT} install_dir)
    add_library(${NAME} STATIC IMPORTED GLOBAL)
    add_library(thirdparty::${NAME} ALIAS ${NAME})
    set_property(TARGET ${NAME} PROPERTY IMPORTED_LOCATION ${install_dir}/lib/${LIB})
    set_property(TARGET ${NAME} PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include)
    add_dependencies(${NAME} ${EXT})
endmacro()

add_subdirectory(doctest)
add_subdirectory(endian)
add_subdirectory(mio)

