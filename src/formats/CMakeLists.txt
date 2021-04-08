set (src
        explode/mz/io.cc
        explode/mz/exe_file.cc
        explode/mz/knowledge_dynamics.cc
        explode/mz/unexepack.cc
        explode/mz/unlzexe.cc
        explode/mz/unpklite.cc
        explode/explode_exe_file.cc

        image/picture.cc
        image/picture_loader.cc
        image/stbi_wrapper.cc
        image/pcx.cc
        image/lbm.cc

        image/amiga/amiga_image.cc
        image/amiga/amiga_chunks.cc

        image/thirdparty/lodepng.cpp
        image/thirdparty/libamivideo/palette.c
        image/thirdparty/libamivideo/screen.c
        image/thirdparty/libamivideo/viewportmode.c

        iff/fourcc.cc
        iff/iff_events.cc
        iff/ea/ea_events.cc

        sound/sound.cc
        )
set (hdr

        ${PROJECT_INCLUDE}/tomb-excavator/formats/explode/mz/explode_exe_file.hh

        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/picture.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/tga.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/bmp.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/gif.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/jpeg.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/lbm.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/pcx.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/png.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/psd.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/tga.hh

        explode/mz/io.hh
        explode/mz/exe_file.hh
        explode/mz/knowledge_dynamics.hh
        explode/mz/struct_reader.hh
        explode/mz/unexepack.hh
        explode/mz/unlzexe.hh
        explode/mz/unpklite.hh

        image/picture_loader.hh
        image/thirdparty/lodepng.h
        image/thirdparty/dr_pcx.h
        image/thirdparty/stb_image.h

        image/thirdparty/libamivideo/palette.h
        image/thirdparty/libamivideo/screen.h
        image/thirdparty/libamivideo/viewportmode.h
        image/thirdparty/libamivideo/amivideotypes.h

        image/amiga/amiga_image.hh
        image/amiga/amiga_chunks.hh

        ${PROJECT_INCLUDE}/tomb-excavator/formats/iff/fourcc.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/iff/iff_events.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/iff/iff_parser.hh

        ${PROJECT_INCLUDE}/tomb-excavator/formats/iff/ea/ea.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/iff/ea/ea_events.hh

        ${PROJECT_INCLUDE}/tomb-excavator/formats/sound/sound.hh


        )

build_te_library(formats
        SOURCES ${src}
        HEADERS ${hdr}
        PUBLIC_LINK
            tombexcavator-bsw
            std::filesystem
            Threads::Threads
        PRIVATE_LINK
            thirdparty::mio
        )
