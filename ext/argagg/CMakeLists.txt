include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        argagg-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/argagg
        GIT_REPOSITORY https://github.com/vietjtnguyen/argagg.git
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
                    -DARGAGG_BUILD_EXAMPLES=OFF -DARGAGG_BUILD_TESTS=OFF -DARGAGG_BUILD_DOCS=OFF
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

ExternalProject_Get_Property(argagg-ext install_dir)
add_library(argagg INTERFACE IMPORTED GLOBAL)
add_library(thirdparty::argagg ALIAS argagg)
set_property(TARGET argagg PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${install_dir}/include/)
