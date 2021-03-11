include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        libaom-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/libaom
        GIT_REPOSITORY https://github.com/link-u/libaom.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
            -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
            -DCMAKE_BUILD_TYPE=$<CONFIG>
            -DCMAKE_POSITION_INDEPENDENT_CODE=ON
            -DAOM_TARGET_CPU=generic
            -DENABLE_EXAMPLES=FALSE -DENABLE_TESTS=FALSE -DENABLE_TOOLS=FALSE
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

IMPORT_THIRDPARTY(libaom-ext libaom libaom.a)