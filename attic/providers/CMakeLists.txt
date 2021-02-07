set(generic_src read_only_fs.cc in_mem_file_ops.cc)
set(generic_hdr read_only_fs.hh single_file_archive.hh in_mem_file_ops.hh)

set(modules generic)

set(src)
set(hdr )

foreach (m ${modules})
    set(mdl_src ${${m}_src})
    set(mdl_hdr ${${m}_hdr})

    foreach (s ${mdl_src})
        list(APPEND src ${CMAKE_CURRENT_SOURCE_DIR}/${s})
    endforeach ()

    foreach (h ${mdl_hdr})
        list(APPEND hdr ${CMAKE_CURRENT_SOURCE_DIR}/${s})
    endforeach ()
    if (m MATCHES "generic")
        source_group("sources" FILES ${mdl_src})
        source_group("headers" FILES ${mdl_hdr})
    else ()
        source_group("${m}\\sources" FILES ${mdl_src})
        source_group("${m}\\headers" FILES ${mdl_hdr})
    endif ()
endforeach ()

add_library(provider SHARED ${src} ${hdr})

add_library(te::provider ALIAS archive)

generate_export_header(provider
        EXPORT_MACRO_NAME PROVIDER_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/providers/provider_api.h
        )
target_compile_features(provider PUBLIC cxx_std_17)

target_include_directories(provider
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        ${CMAKE_SOURCE_DIR}/src
        )

target_compile_options(provider PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -fvisibility=hidden -fvisibility-inlines-hidden>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

target_compile_definitions(provider
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )
target_link_libraries(provider PUBLIC archive)
# ===============================================================================================

add_subdirectory(stargunner)
add_subdirectory(mystic_towers)
add_subdirectory(archon_ultra)
