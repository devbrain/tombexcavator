set (src
        io/input.cc
        io/output.cc
        io/io_base.cc

        mzexplode/exe_file.cc
        mzexplode/knowledge_dynamics.cc
        mzexplode/unexepack.cc
        mzexplode/unlzexe.cc
        mzexplode/unpklite.cc
        )
set (hdr
        io/input.hh
        io/output.hh
        io/io_base.hh
        io/io.hh

        mzexplode/exe_file.hh
        mzexplode/knowledge_dynamics.hh
        mzexplode/unexepack.hh
        mzexplode/unlzexe.hh
        mzexplode/unpklite.hh
        mzexplode/struct_reader.hh
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
          thirdparty::endian
          std::filesystem
          Threads::Threads

        )

target_compile_options(tombexcavator-formats PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)