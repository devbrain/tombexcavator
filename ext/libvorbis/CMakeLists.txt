include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        libvorbis-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/libvorbis
        GIT_REPOSITORY https://github.com/xiph/vorbis.git
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

IMPORT_THIRDPARTY(libvorbis-ext libvorbis libvorbis.a)
IMPORT_THIRDPARTY(libvorbis-ext libvorbis-enc libvorbisenc.a)
IMPORT_THIRDPARTY(libvorbis-ext libvorbis-file libvorbisfile.a)