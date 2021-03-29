include(ExternalProject)
find_package(Git REQUIRED)


ExternalProject_Add(
        dlfcn-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/dlfcn
        GIT_REPOSITORY https://github.com/dlfcn-win32/dlfcn-win32.git
        UPDATE_COMMAND ""
        INSTALL_DIR ${GLOBAL_OUTPUT_PATH}
        CMAKE_ARGS  -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR> -DCMAKE_BUILD_TYPE=$<CONFIG> -DBUILD_SHARED_LIBS=OFF 
        BUILD_COMMAND  ${CMAKE_COMMAND} --build <BINARY_DIR> 
)

IMPORT_THIRDPARTY(dlfcn-ext dlfcn-win32 dl.lib)



