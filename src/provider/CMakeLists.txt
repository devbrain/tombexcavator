
set (src
        vfs/directory.cc
        vfs/file_system.cc

        physfs/directory.cc
        vfs_registry.cc
        )

set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs/directory.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs/file_system.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/physfs/directory.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/file_types.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs_registry.hh
        )


if (WIN32)
    set (DL_LIBS thirdparty::dlfcn-win32)
else()
    set (DL_LIBS ${CMAKE_DL_LIBS})
endif()

build_te_library(provider
        SOURCES ${src}
        HEADERS ${hdr}

        PUBLIC_LINK
            tombexcavator-formats
            tombexcavator-bsw
            ${DL_LIBS}
        )

