set (src
        picture_export.cc
        sprite_group_export.cc
        xml_writer.cc
        file_type_to_string.cc
        romfont.cc
        canvas_utils.cc
        )
set (hdr
        ${PROJECT_INCLUDE}/tomb-excavator/export/transformer.hh
        ${PROJECT_INCLUDE}/tomb-excavator/export/picture_export.hh
        ${PROJECT_INCLUDE}/tomb-excavator/export/sprite_group_export.hh
        ${PROJECT_INCLUDE}/tomb-excavator/export/xml_writer.hh
        ${PROJECT_INCLUDE}/tomb-excavator/export/file_type_to_string.hh
        ${PROJECT_INCLUDE}/tomb-excavator/export/romfont.hh

        canvas_utils.hh
        )


build_te_library(export
        SOURCES ${src}
        HEADERS ${hdr}

        PUBLIC_LINK
        tombexcavator-provider
        tombexcavator-formats
        tombexcavator-bsw
        )
