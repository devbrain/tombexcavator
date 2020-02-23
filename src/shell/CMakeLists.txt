set (src main.cc)
set (hdr)

add_executable(shell ${src} ${hdr})

target_include_directories(shell
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/src>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        )

target_compile_definitions(shell
        PRIVATE
        ONYX_MODULE_NAME=shell
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )

target_compile_features(shell PUBLIC cxx_std_17)

target_link_libraries(shell PUBLIC
        Threads::Threads

        vfs::vfs
        bsw::bsw
)

add_dependencies(shell linenoise_ext vfs_ext)


