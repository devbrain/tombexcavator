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
        image/thirdparty/lodepng.cpp

        sound/sound.cc
        )
set (hdr

        ${PROJECT_INCLUDE}/tomb-excavator/formats/explode/mz/explode_exe_file.hh

        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/picture.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/tga.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/bmp.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/gif.hh
        ${PROJECT_INCLUDE}/tomb-excavator/formats/image/jpeg.hh
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

        ${PROJECT_INCLUDE}/tomb-excavator/formats/sound/sound.hh
        )

build_te_library(formats
        SOURCES ${src}
        HEADERS ${src}
        PUBLIC_LINK
            tombexcavator-bsw
            std::filesystem
            Threads::Threads
        PRIVATE_LINK
            thirdparty::mio
        )
