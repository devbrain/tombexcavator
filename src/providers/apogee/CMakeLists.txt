set (src
        prographx.cc
        )

set(hdr
        ${PROJECT_INCLUDE}/tomb-excavator/games/apogee/prographx.hh

        ega.hh
        )

add_library(tombexcavator-games-apogee SHARED ${src} ${hdr})

generate_export_header(tombexcavator-games-apogee
        BASE_NAME GAMES_APOGEE_EXPORT
        EXPORT_MACRO_NAME GAMES_APOGEE_API
        EXPORT_FILE_NAME ${PROJECT_CONFIG_INCLUDE}/tomb-excavator/games-apogee_export.h
        STATIC_DEFINE GAMES_APOGEE_BUILT_AS_STATIC
        )

target_include_directories(tombexcavator-games-apogee
        PUBLIC ${PROJECT_CONFIG_INCLUDE} ${PROJECT_INCLUDE}
        PRIVATE ${PROJECT_INCLUDE_PRIVATE}
        )

set_target_properties(tombexcavator-games-apogee PROPERTIES CXX_STANDARD 17)

target_link_libraries(tombexcavator-games-apogee
        PUBLIC
        tombexcavator-games-common
        )
