# ------------------------------------------------------------------
set(generic_src
        main.cc
        md5.c
        )

set(generic_hdr
        md5.h
        )

set(archive_src
        archive/explode/explode_test.cc
        )

set(modules
        generic
        archive
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
        tombexcavator-formats
        thirdparty::doctest
        )
target_include_directories(unittest PRIVATE ${PROJECT_ROOT}/src)



