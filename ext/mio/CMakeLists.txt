include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        mio-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/mio
        GIT_REPOSITORY https://github.com/mandreyel/mio.git
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG>
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(mio-ext install_dir)
add_library(mio INTERFACE IMPORTED GLOBAL)
add_library(thirdparty::mio ALIAS mio)
set_property(TARGET mio PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include/)


