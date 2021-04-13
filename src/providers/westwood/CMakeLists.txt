set (src
        westwood_data_loader.cc
        westwood_pak_loader.cc
        westwood_entry_loader.cc

        westwood_cps.cc
        westwood_compression.cc
        )

set(hdr
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_data_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_pak_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_entry_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_cps.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_deps.hh

        westwood_compression.hh
        )

build_te_library(games-westwood
        SOURCES ${src}
        HEADERS ${hdr}

        PUBLIC_LINK
            tombexcavator-games-common
)
