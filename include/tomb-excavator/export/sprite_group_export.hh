//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_SPRITE_GROUP_EXPORT_HH
#define TOMBEXCAVATOR_SPRITE_GROUP_EXPORT_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/export/transformer.hh>
#include <tomb-excavator/provider/dto/video/sprite.hh>
#include <tomb-excavator/provider/dto/video/coord.hh>
#include <vector>
#include <filesystem>

namespace exporter
{

    // ==============================================================================
    EXPORT_API void to_png(const provider::dto::tile_sheet& ts, const std::filesystem::path& oname);

    EXPORT_API void to_tsx(const provider::dto::tile_sheet& ts,
                           const std::string& tileset_name,
                           const std::filesystem::path& oname_tmx,
                           const std::filesystem::path& oname_png);
}

#endif //TOMBEXCAVATOR_SPRITE_GROUP_EXPORT_HH
