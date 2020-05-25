#include "output.hh"
#include "thirdparty/lodepng/lodepng.h"
#include <vector>
#include <algorithm>
#include <stdio.h>

namespace archive
{
    bool save_pixels(const std::string& fname,
                     const unsigned char* pixels, std::size_t w, std::size_t h,
                     const unsigned char* pal, size_t num_colors)
    {
        LodePNGState state;
        lodepng_state_init(&state);

        state.info_raw.colortype = LCT_PALETTE;
        state.info_raw.bitdepth = 8;
        state.info_png.color.colortype = LCT_PALETTE;
        state.info_png.color.bitdepth = 8;
        for (std::size_t i = 0; i < num_colors; i++)
        {
            const uint8_t r = pal[3*i + 0] * 4;
            const uint8_t g = pal[3*i + 1] * 4;
            const uint8_t b = pal[3*i + 2] * 4;
            lodepng_palette_add(&state.info_raw, r, g, b, 0xFF);
            lodepng_palette_add(&state.info_png.color, r, g, b, 0xFF);
        }
        unsigned char* out;
        size_t outsize;
        lodepng_encode(&out, &outsize, pixels, w, h, &state);
        auto error = state.error;
        lodepng_state_cleanup(&state);
        if (!error)
        {
            FILE* f = fopen(fname.c_str(), "wb");
            fwrite(out, outsize, 1, f);
            fclose(f);
            free(out);
            return true;
        }
        return false;
    }
    // ======================================================================================================
    bool save_palette(const std::string& fname, const unsigned char* pal, size_t num_colors)
    {
        constexpr std::size_t columns = 8;
        std::size_t rows = num_colors / columns;
        if (num_colors % columns)
        {
            rows++;
        }
        constexpr size_t side = 16;
        const size_t px_width = side * columns;
        const size_t px_height = side * rows;
        std::vector<unsigned char> pixels(px_width * px_height);

        std::size_t r = 0;
        std::size_t c = 0;
        for (std::size_t col = 0; col < num_colors; col++)
        {
            if (c == columns)
            {
                c = 0;
                r++;
            } else
            {
                c++;
            }
            for (std::size_t y = 0; y < side; y++)
            {
                for (std::size_t x = 0; x < side; x++)
                {
                    const std::size_t xpos = c * side + x;
                    const std::size_t ypos = r * side + y;
                    std::size_t idx = px_width * ypos + xpos;
                    pixels[idx] = col & 0xFF;
                }
            }
        }
        return save_pixels(fname, pixels.data(), px_width, px_height, pal, num_colors);
    }
    // ============================================================================================
    bool save_palette(const std::string& fname, const std::vector<unsigned char>& pal)
    {
        return save_palette(fname, pal.data(), pal.size());
    }
    // ============================================================================================
    bool save_pixels(const std::string& fname,
                     const std::vector<unsigned char>& pixels, std::size_t w, std::size_t h,
                     const std::vector<unsigned char>& pal)
    {
        return save_pixels(fname, pixels.data(), w, h, pal.data(), pal.size());
    }
    // ============================================================================================
    pixels_frame::pixels_frame(std::size_t width, std::size_t height)
    : pixels(width*height),
    w (width),
    h (height)
    {

    }
    // ============================================================================================
    bool save_pixels(const std::string& fname,
                     const pixels_frame& frame,
                     const std::vector<unsigned char>& pal)
    {
        return save_pixels(fname, frame.pixels, frame.w, frame.h, pal);
    }
    // =============================================================================================
    bool save_pixels(const std::string& fname,
                     const std::vector<pixels_frame>& frames,
                     const std::vector<unsigned char>& pal)
    {
        std::size_t px_width = 0;
        std::size_t px_height = 0;
        std::for_each(frames.begin(), frames.end(), [&px_width, &px_height](const pixels_frame& f) {
           px_width = std::max(px_width, f.w);
           px_height += f.h;
        });
        pixels_frame img(px_width, px_height);

        std::size_t r = 0;
        for (const auto& f : frames)
        {
            for (std::size_t y = 0; y<f.h; y++)
            {
                for (std::size_t x = 0; x<f.w; x++)
                {
                    auto src_idx = f.w*y + x;
                    auto dst_idx = px_width*r + x;
                    img.pixels[dst_idx] = f.pixels[src_idx];
                }
                r++;
            }
        }
        return save_pixels(fname, img, pal);
    }
} // ns archive
