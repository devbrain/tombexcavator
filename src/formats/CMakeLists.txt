set (src
        exceptions.cc

        explode/mz/io.cc
        explode/mz/exe_file.cc
        explode/mz/knowledge_dynamics.cc
        explode/mz/unexepack.cc
        explode/mz/unlzexe.cc
        explode/mz/unpklite.cc
        explode/explode_exe_file.cc

        image/picture.cc
        image/stbi_wrapper.cc
        image/pcx.cc
        image/thirdparty/lodepng.cpp
        )
set (hdr
        exceptions.hh
        io/byte_order.hh

        explode/mz/io.hh
        explode/mz/exe_file.hh
        explode/mz/knowledge_dynamics.hh
        explode/mz/struct_reader.hh
        explode/mz/unexepack.hh
        explode/mz/unlzexe.hh
        explode/mz/unpklite.hh
        explode/explode_exe_file.hh

        image/picture.hh

        image/tga.hh
        image/bmp.hh
        image/gif.hh
        image/jpeg.hh
        image/pcx.hh
        image/png.hh
        image/psd.hh
        image/tga.hh

        image/thirdparty/lodepng.h
        image/thirdparty/dr_pcx.h
        image/thirdparty/stb_image.h

        )

add_library(tombexcavator-formats SHARED ${src} ${hdr})

generate_export_header(tombexcavator-formats
             BASE_NAME FORMATS_EXPORT
             EXPORT_MACRO_NAME FORMATS_API
             EXPORT_FILE_NAME tombexcavator-formats_export.h
             STATIC_DEFINE FORMATS_BUILT_AS_STATIC
)

target_include_directories(tombexcavator-formats PUBLIC ${PROJECT_INCLUDE} ${CMAKE_CURRENT_BINARY_DIR})

set_target_properties(tombexcavator-formats PROPERTIES CXX_STANDARD 17)

target_link_libraries(tombexcavator-formats PUBLIC
          thirdparty::mio
          std::filesystem
          Threads::Threads
        )

target_compile_options(tombexcavator-formats PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)
