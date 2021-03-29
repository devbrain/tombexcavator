include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        libogg-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/libogg
        GIT_REPOSITORY https://github.com/xiph/ogg.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

IMPORT_THIRDPARTY(libogg-ext libogg libogg.a)