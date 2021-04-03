set (src
        ega_palette.cc
        vga_palette.cc
        load_palette.cc

        loaders/base_archive_loader.cc
        loaders/archive_loader.cc
        loaders/picture_loader.cc
        loaders/sprite_loader.cc
        loaders/palette_loader.cc
        loaders/fat.cc
        loaders/archive_entry_loader.cc

        vfs/root_directory.cc
        vfs/virtual_fs.cc
        vfs/data_loader.cc
        vfs/directory_adaptor.cc
        )

set(hdr
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/ega_palette.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/vga_palette.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/load_palette.hh

        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/single_entry_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/picture_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/sprite_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/base_archive_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/archive_entry_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/archive_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/palette_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/loader_functions.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/loaders/fat.hh

        ${PROJECT_INCLUDE}/tomb-excavator/games/common/vfs/data_loader.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/vfs/root_directory.hh
        ${PROJECT_INCLUDE}/tomb-excavator/games/common/vfs/virtual_fs.hh

        vfs/directory_adaptor.hh
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
        tombexcavator-provider
        tombexcavator-bsw
        thirdparty::mio
        )
