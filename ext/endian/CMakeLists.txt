include(ExternalProject)
find_package(Git REQUIRED)

ExternalProject_Add(
        endian-ext
        PREFIX ${CMAKE_BINARY_DIR}/thirdparty/endian
        GIT_REPOSITORY https://github.com/mandreyel/endian.git
        GIT_TAG master
        GIT_PROGRESS TRUE
        TIMEOUT 10
        UPDATE_COMMAND ""
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
        LOG_DOWNLOAD ON
)

# Expose required variable (DOCTEST_INCLUDE_DIR) to parent scope
ExternalProject_Get_Property(endian-ext source_dir)
message("**** ${source_dir}")
file(MAKE_DIRECTORY ${source_dir}/endian)
add_library(endian INTERFACE IMPORTED GLOBAL)
add_library(thirdparty::endian ALIAS endian)
set_property(TARGET endian PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir})