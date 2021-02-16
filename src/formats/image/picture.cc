//
// Created by igor on 14/02/2021.
//

#include <fstream>
#include <ios>
#include "formats/image/picture.hh"
#include "formats/image/picture_loader.hh"
#include <mio/mmap.hpp>
#include "formats/image/thirdparty/lodepng.h"

namespace formats::image
{
    bool save_to_png(const picture& pic, const std::filesystem::path& path)
    {
        const LodePNGColorType color_type = (pic.bpp == 3) ? LCT_RGB : LCT_RGBA;
        unsigned char* out;
        size_t outsize;
        if (0 != lodepng_encode_memory(&out, &outsize, (unsigned char*)pic.pixels.data(), pic.width, pic.height, color_type, 8))
        {
            return false;
        }

        auto file = std::fstream(path, std::ios::out | std::ios::binary);

        if (!file)
        {
            free(out);
            return false;
        }

        file.write((char*)out, outsize);
        file.close();
        free(out);
        return true;
    }
    // ------------------------------------------------------------------------------------------------------------
    picture load_picture(const std::filesystem::path& path)
    {
        mio::mmap_source mmap(path.string());
        try
        {
            return load_picture(mmap.data(), mmap.size());
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("unknown image format " + path.string());
        }
    }
    // ------------------------------------------------------------------------------------------------------------
    picture load_picture(const char* data, std::size_t size)
    {
        picture pic;
        if (!picture_loader::instance().load(data, size, pic))
        {
            throw std::runtime_error("unknown image format");
        }
        return pic;
    }
}