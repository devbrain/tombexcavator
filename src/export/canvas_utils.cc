//
// Created by igor on 21/03/2021.
//

#include "canvas_utils.hh"

namespace exporter
{
    provider::dto::coord draw_on_canvas(formats::image::picture& opic, int x0, int y0,
                                        const provider::dto::sprite& sprite,
                                        const provider::dto::palette& pal)
    {
        const auto& d = sprite.dim();
        const auto& canvas = sprite.get_canvas();
        const auto& bm = sprite.get_bitmask();

        for (int y=0; y<d.h(); y++)
        {
            for (int x=0; x<d.w(); x++)
            {
                const auto color = canvas.get(x, y);
                auto [r,g,b,a] = pal[color];
                if (bm)
                {

                    a = bm->is_set(x, y) ? 0xFF : 0x00;
                }
                else
                {
                    a = 0x0FF;
                }
                opic.put(x0 + x, y0 + y, r, g, b, a);
            }
        }
        return {{x0, y0}, d};
    }
    // -----------------------------------------------------------------------------------------------------------------
    provider::dto::coord draw_on_canvas(formats::image::picture& opic, int x0, int y0, font fnt,
                                        const std::string& str, const provider::dto::rgba& color)
    {
        int h = 0;
        int x = 0;
        for (std::size_t i=0; i<str.size(); i++)
        {
            auto ch = str[i];
            auto pos = draw_on_canvas(opic, x0 + x, y0, fnt, ch, color);
            x += pos.dim().w();
            h = pos.dim().h();
        }
        return {{x0, y0}, {x, h}};
    }
    // -----------------------------------------------------------------------------------------------------------------
    provider::dto::coord draw_on_canvas(formats::image::picture& opic, int x0, int y0, font fnt,
                                        char ch, const provider::dto::rgba& color)
    {
        const auto& glyph = get_glyph(fnt, ch);
        for (std::size_t y=0; y<glyph.size(); y++)
        {
            const auto& row = glyph[y];
            for (int x=0; x<8; x++)
            {
                provider::dto::rgba pen {0, 0, 0, 0xFF};
                if (row[x])
                {
                    pen = color;
                }
                opic.put(x0 + x, y0 + y, pen.r, pen.g, pen.b, pen.a);
            }
        }
        return {{x0, y0}, {8, (int)glyph.size()}};
    }
}
