//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_EXPORT_HH
#define TOMBEXCAVATOR_PICTURE_EXPORT_HH

#include <tomb-excavator/export-export.h>
#include <tomb-excavator/provider/dto/video/picture.hh>
#include <filesystem>

namespace exporter
{
    void EXPORT_API to_png(const provider::dto::picture& pic, const std::filesystem::path& opath);
}

#endif //TOMBEXCAVATOR_PICTURE_EXPORT_HH
