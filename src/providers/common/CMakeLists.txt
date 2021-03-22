set (src
        ega_palette.cc

        loaders/data_loader.cc
        loaders/archive_loader.cc
        loaders/picture_loader.cc
        loaders/sprite_loader.cc

        flat_provider/root_directory.cc
        flat_provider/flat_provider.cc
        )

set(hdr
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/ega_palette.hh

        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/data_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/picture_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/sprite_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/archive_loader.hh

        ${PROJECT_INCLUDE}/tomb-excavator/games/common/flat_provider/root_directory.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/flat_provider/flat_provider.hh
        )

add_library(tombexcavator-games-common SHARED ${src} ${hdr})

generate_export_header(tombexcavator-games-common
        BASE_NAME GAMES_COMMON_EXPORT
        EXPORT_MACRO_NAME GAMES_COMMON_API
        EXPORT_FILE_NAME ${PROJECT_CONFIG_INCLUDE}/tomb-excavator/export-games-common.h
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
        thirdparty::mio
        )
