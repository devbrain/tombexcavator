//
// Created by igor on 29/03/2021.
//

#ifndef TOMBEXCAVATOR_PALETTE_EXPORT_HH
#define TOMBEXCAVATOR_PALETTE_EXPORT_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/provider/dto/video/palette.hh>
#include <filesystem>

namespace exporter
{
    void EXPORT_API to_png(const provider::dto::palette& pal, const std::filesystem::path& opath);
}

#endif //TOMBEXCAVATOR_PALETTE_EXPORT_HH
