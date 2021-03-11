//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/export/picture_export.hh>
#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace exporter
{
    void to_png(const provider::dto::picture& pic, const std::filesystem::path& opath)
    {
        const auto w = pic.dim().w();
        const auto h = pic.dim().h();
        formats::image::picture opic(w, h, false);
        for (int y=0; y<h; y++)
        {
            for (int x=0; x<w; x++)
            {
                auto color = pic.get(x, y);
                auto [r,g,b,a] = pic.pal()[color];
                opic.put(x, y, r, g, b, a);
            }
        }
        if (!formats::image::save_to_png(opic, opath))
        {
            RAISE_EX("Failed to save ", opath);
        }
    }
} // ns
