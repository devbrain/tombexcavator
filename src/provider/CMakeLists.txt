set (src
        dto/video/bitmask.cc
        dto/video/coord.cc
        dto/video/canvas.cc
        dto/video/palette.cc
        dto/video/sprite.cc
        dto/video/picture.cc

        vfs/directory.cc
        vfs/file_system.cc

        physfs/directory.cc
        physfs/data_loader.cc

        vfs_registry.cc


        )

set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs/directory.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs/file_system.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/physfs/directory.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/physfs/data_loader.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/file_types.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/vfs_registry.hh

        ${PROJECT_INCLUDE}/tomb-excavator/provider/dto/video/coord.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/dto/video/palette.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/dto/video/canvas.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/dto/video/bitmask.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/dto/video/sprite.hh
        ${PROJECT_INCLUDE}/tomb-excavator/provider/dto/video/picture.hh


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

