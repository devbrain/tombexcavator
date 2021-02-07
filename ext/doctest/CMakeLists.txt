include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        doctest-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/doctest
        GIT_REPOSITORY https://github.com/onqtam/doctest.git
        GIT_TAG 2.4.0
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(doctest-ext source_dir)
file(MAKE_DIRECTORY ${source_dir}/doctest)
add_library(doctest INTERFACE IMPORTED GLOBAL)
add_library(thirdparty::doctest ALIAS doctest)
set_property(TARGET doctest PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/doctest)