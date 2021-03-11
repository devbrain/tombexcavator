set (src
        crystal_caves_provider.cc)
set (hdr)

add_library(crystal_caves SHARED ${src} ${hdr})
set_target_properties(tombexcavator-provider PROPERTIES CXX_STANDARD 17)

target_link_libraries(crystal_caves
        PRIVATE
        tombexcavator-provider
        tombexcavator-games-apogee
        tombexcavator-games-common
        )

target_compile_options(crystal_caves PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden
        $<$<COMPILE_LANGUAGE:CXX>:-fvisibility-inlines-hidden> >
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)