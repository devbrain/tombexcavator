#include "output.hh"
#include "thirdparty/lodepng/lodepng.h"
#include <vector>
#include <stdio.h>

namespace archive
{
    bool save_pixels(const std::string& fname,
                     const unsigned char* pixels, std::size_t w, std::size_t h,
                     const palette& pal)
    {
        LodePNGState state;
        lodepng_state_init(&state);

        state.info_raw.colortype = LCT_PALETTE;
        state.info_raw.bitdepth = 8;
        state.info_png.color.colortype = LCT_PALETTE;
        state.info_png.color.bitdepth = 8;
        for (std::size_t i = 0; i < pal.size(); i++)
        {
            const uint8_t r = pal[i].r;
            const uint8_t g = pal[i].g;
            const uint8_t b = pal[i].b;
            const uint8_t a = pal[i].a;
            lodepng_palette_add(&state.info_raw, r, g, b, a);
            lodepng_palette_add(&state.info_png.color, r, g, b, a);
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
    bool save_palette(const std::string& fname, const palette& pal)
    {
        constexpr std::size_t columns = 8;
        auto num_colors = pal.size();
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
                    pixels[idx] = (unsigned char)(col & (std::size_t)0xFF);
                }
            }
        }
        return save_pixels(fname, pixels.data(), px_width, px_height, pal);
    }
} // ns archive
