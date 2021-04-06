SET(TARGET_NAME pfr)

include(ExternalProject)
ExternalProject_Add(
        pfr-ext
        GIT_REPOSITORY https://github.com/boostorg/pfr.git
        CONFIGURE_COMMAND ""
        BUILD_COMMAND ""
        INSTALL_COMMAND ""
)
ExternalProject_Get_Property(pfr-ext source_dir)
add_library(pfr INTERFACE IMPORTED GLOBAL)
add_library(thirdparty::pfr ALIAS pfr)
set_property(TARGET pfr PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${source_dir}/include/)