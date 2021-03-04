set (src
        te-cli.cc
        params.cc
        )

set(hdr
        params.hh
        )

add_executable(te-cli ${src} ${hdr})

target_link_libraries(te-cli PRIVATE
        thirdparty::argagg
        tombexcavator-provider
        )

set_target_properties(te-cli PROPERTIES CXX_STANDARD 17)

target_compile_options(te-cli PRIVATE
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>: -Wall -Wextra -pedantic -fvisibility=hidden>
        $<$<CXX_COMPILER_ID:MSVC>: "/W4" >)