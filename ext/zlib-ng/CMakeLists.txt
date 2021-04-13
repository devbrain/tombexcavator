include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        zlibng-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/zlibng
        GIT_REPOSITORY https://github.com/zlib-ng/zlib-ng.git
        GIT_PROGRESS TRUE
        GIT_TAG develop
        TIMEOUT 10
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DCMAKE_BUILD_TYPE=$<CONFIG>
        -DZLIB_COMPAT=ON
        -DZLIB_ENABLE_TESTS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR>
)

if (MSVC)
    set (LIBZNG z.lib)
else()
    set (LIBZNG libz.a)
endif()

IMPORT_THIRDPARTY(zlibng-ext zlibng ${LIBZNG})