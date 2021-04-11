set (src
        dune2.cc
        dune_2_pak_reader.cc
        emc.cc
        chunked_file.cc)
set (hdr
        dune_2_pak_reader.hh
        emc.hh
        chunked_file.hh)

add_library(dune2 SHARED ${src} ${hdr})
set_target_properties(dune2 PROPERTIES CXX_STANDARD 17)

target_link_libraries(dune2
        PRIVATE
        tombexcavator-provider
        tombexcavator-games-westwood
        tombexcavator-games-common
        tombexcavator-formats
        )

target_compile_options(dune2 PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)