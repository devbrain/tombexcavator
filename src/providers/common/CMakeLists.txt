set (src
        coord.cc
        palette.cc
        tile_sheet.cc
        canvas.cc
        ega_palette.cc
        bitmask.cc
        )

set(hdr
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/coord.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/palette.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/tile_sheet.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/canvas.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/ega_palette.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/bitmask.hh
        )

add_library(tombexcavator-games-common SHARED ${src} ${hdr})

generate_export_header(tombexcavator-games-common
        BASE_NAME GAMES_COMMON_EXPORT
        EXPORT_MACRO_NAME GAMES_COMMON_API
        EXPORT_FILE_NAME ${PROJECT_CONFIG_INCLUDE}/tomb-excavator/games-common_export.h
        STATIC_DEFINE GAMES_COMMON_BUILT_AS_STATIC
        )

target_include_directories(tombexcavator-games-common
        PUBLIC ${PROJECT_CONFIG_INCLUDE} ${PROJECT_INCLUDE}
        PRIVATE ${PROJECT_INCLUDE_PRIVATE}
        )

set_target_properties(tombexcavator-games-common PROPERTIES CXX_STANDARD 17)

target_link_libraries(tombexcavator-games-common
        PUBLIC
        tombexcavator-formats
        tombexcavator-bsw
        )
