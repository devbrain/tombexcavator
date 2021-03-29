# ------------------------------------------------------------------
set(generic_src
        main.cc
        md5.c
        )

set(generic_hdr
        md5.h
        )

set(formats_src
        formats/explode/mz/explode_test.cc
        )

set(bsw_src
        bsw/io/memory_stream.cc
        bsw/io/binary_reader_writer.cc
        bsw/io/base_64_test.cc
        bsw/string_tokenizer_test.cc
        bsw/string_utils_test.cc
        )
set (export_src
        export/xml_writer_test.cc
        )

set(modules
        generic
        formats
        bsw
        export
        )

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

add_executable(unittest ${src} ${hdr})

target_link_libraries(unittest PRIVATE
        tombexcavator-export
        tombexcavator-formats
        tombexcavator-bsw
        thirdparty::doctest
        )
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)



