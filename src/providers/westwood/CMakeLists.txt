set (src
        westwood_data_loader.cc
        westwood_pak_loader.cc
        )

set(hdr
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_data_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/westwood/westwood_pak_loader.hh
        )

build_te_library(games-westwood
        SOURCES ${src}
        HEADERS ${hdr}

        PUBLIC_LINK
            tombexcavator-games-common
)
