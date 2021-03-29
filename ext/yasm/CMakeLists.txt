include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        yasm-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/yasm
        GIT_REPOSITORY https://github.com/yasm/yasm.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=$<CONFIG>
        -DYASM_BUILD_TESTS=OFF
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(yasm-ext install_dir)
add_executable(yasm IMPORTED GLOBAL)
add_executable(thirdparty::yasm ALIAS yasm)
set_property(TARGET yasm PROPERTY IMPORTED_LOCATION ${install_dir}/lib/${LIB})
add_dependencies(yasm yasm-ext)