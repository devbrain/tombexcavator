//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_PICTURE_HH
#define TOMBEXCAVATOR_PICTURE_HH

#include <tombexcavator-formats_export.h>
#include <vector>
#include <filesystem>

namespace formats::image
{
    struct FORMATS_API picture
    {
        unsigned width;
        unsigned height;
        int bpp; // 3 for RGB and 4 for RGBA

        std::vector<char> pixels;
    };

    FORMATS_API bool save_to_png(const picture& pic, const std::filesystem::path& path);
    FORMATS_API picture load_picture(const std::filesystem::path& path);
    FORMATS_API picture load_picture(const char* data, std::size_t size);
} // ns

#endif //TOMBEXCAVATOR_PICTURE_HH
