//
// Created by igor on 14/02/2021.
//

#include <fstream>
#include <iterator>
#include <ios>
#include "tomb-excavator/formats/image/picture.hh"
#include "formats/image/picture_loader.hh"
#include <tomb-excavator/bsw/exceptions.hh>

#if defined(_MSC_VER)
#pragma warning( push )
#pragma warning( disable : 4244 )
#endif

#include <mio/mmap.hpp>

#if defined(_MSC_VER)
#pragma warning( pop )
#endif

#include "formats/image/thirdparty/lodepng.h"
#include <cstring>

namespace formats::image
{
    indexed_color_mapper::indexed_color_mapper(const rgb_palette& pal)
    : m_pal{pal}
    {

    }
    // -----------------------------------------------------------------------------------------------------
    indexed_color_mapper::indexed_color_mapper(const rgba_palette& pal)
    : m_pal{pal}
    {

    }
    // -----------------------------------------------------------------------------------------------------
    indexed_color_mapper::indexed_color_mapper(const rgb_palette& pal, uint8_t key_color)
    : m_pal{pal}, m_key_color(key_color)
    {

    }
    // -----------------------------------------------------------------------------------------------------
    indexed_color_mapper::indexed_color_mapper(const rgba_palette& pal, uint8_t key_color)
            : m_pal{pal}, m_key_color(key_color)
    {

    }
    // -----------------------------------------------------------------------------------------------------
    rgba indexed_color_mapper::operator[] (uint8_t color) const
    {
        rgba res;
        if (const auto* pal = std::get_if<rgba_palette>(&m_pal))
        {
            if (color >= pal->size())
            {
                RAISE_EX("Color index out of range");
            }
            res = (*pal)[color];
        }
        else
        {
            const auto& pal_rgb = std::get<rgb_palette>(m_pal);
            if (color >= pal_rgb.size())
            {
                RAISE_EX("Color index out of range");
            }
            const rgb& x = pal_rgb[color];
            res.r = x.r;
            res.g = x.g;
            res.b = x.b;
            res.a = 0xFF;
        }
        if (m_key_color && color == *m_key_color)
        {
            res.a = 0x00;
        }
        return res;
    }
    // ===========================================================================================
    picture::picture(unsigned w, unsigned h, bool is_rgba)
    : width(w),
    height(h),
    bpp(is_rgba ? 4 : 3)
    {
        pixels.resize(bpp*width*height);
    }
    // -------------------------------------------------------------------------------------------
    void picture::put(unsigned x, unsigned y, uint8_t r, uint8_t g, uint8_t b, uint8_t a)
    {

        const std::size_t idx = (y*width + x)*bpp;
        if (idx + bpp > pixels.size())
        {
            RAISE_EX("Coord (", x, ",", y, ") >= width ", width, " height ", height);
        }
        pixels[idx+0] = r;
        pixels[idx+1] = g;
        pixels[idx+2] = b;
        if (bpp == 4)
        {
            pixels[idx+3] = a;
        }
    }
    // -------------------------------------------------------------------------------------------
    void picture::put(unsigned x, unsigned y, uint8_t color, const indexed_color_mapper& mapper)
    {
        put(x, y, mapper[color]);
    }
    // -------------------------------------------------------------------------------------------
    void picture::put(unsigned x, unsigned y, const rgb& color)
    {
        put (x, y, color.r, color.g, color.b, 0xFF);
    }
    // -------------------------------------------------------------------------------------------
    void picture::put(unsigned x, unsigned y, const rgba& color)
    {
        put (x, y, color.r, color.g, color.b, color.a);
    }
    // =================================================================================================
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
    bool save_to_png(const picture& pic, std::vector<char>& outvec)
    {
        const LodePNGColorType color_type = (pic.bpp == 3) ? LCT_RGB : LCT_RGBA;
        unsigned char* out;
        size_t outsize;
        if (0 != lodepng_encode_memory(&out, &outsize, (unsigned char*)pic.pixels.data(), pic.width, pic.height, color_type, 8))
        {
            return false;
        }
        outvec.resize(outsize);
        std::memcpy(outvec.data(), out, outsize);
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
        catch (const std::exception& )
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
    // ------------------------------------------------------------------------------------------------------------
    picture load_picture(std::istream& is)
    {
        std::vector<char> data((std::istreambuf_iterator<char>(is)),
                               std::istreambuf_iterator<char>());
        return load_picture(data.data(), data.size());
    }
}