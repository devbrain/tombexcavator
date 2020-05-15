include(GenerateExportHeader)

include(TestBigEndian)
TEST_BIG_ENDIAN(ARCHIVE_ARCH_IS_BIG_ENDIAN)

include(CheckTypeSize)
check_type_size("int64_t" SIZEOF_INT64_T LANGUAGE CXX)

if (SIZEOF_INT64_T EQUAL "8")
    set(ARCHIVE_HAVE_INT64 "int64_t")
endif ()
# ------------------------------------------------------------------
set (explode_src
        explode/exe_file.cc
        explode/unpklite.cc
        explode/unexepack.cc
        explode/unlzexe.cc
        explode/knowledge_dynamics.cc
        )

set (explode_hdr
        explode/exe_file.hh
        explode/unpklite.hh
        explode/struct_reader.hh
        explode/unexepack.hh
        explode/unlzexe.hh
        explode/knowledge_dynamics.hh
        )

set(generic_src
        archive.cc
        io.cc
        exceptions.cc
        )
set(generic_hdr
        byte_order.hh
        io.hh
        exceptions.hh
        )

set(modules generic explode)

set(src)
set(hdr)

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

add_library(archive SHARED ${src} ${hdr})

add_library(te::archive ALIAS archive)

generate_export_header(archive
        EXPORT_MACRO_NAME ARCHIVE_API
        EXPORT_FILE_NAME ${CMAKE_BINARY_DIR}/include/archive/archive_api.h
        )
configure_file(archive_config.h.in ${CMAKE_BINARY_DIR}/include/archive/archive_config.h)
target_compile_features(archive PUBLIC cxx_std_17)

target_include_directories(archive
        PUBLIC
        $<INSTALL_INTERFACE:include>
        $<BUILD_INTERFACE:${PROJECT_ROOT}/include>
        $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
        )

target_compile_options(archive PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
        -Wall -fvisibility=hidden -fvisibility-inlines-hidden>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)

target_compile_definitions(archive
        PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        _CRT_SECURE_NO_DEPRECATE
        _CRT_NONSTDC_NO_DEPRECATE
        NOMINMAX >
        )

      
