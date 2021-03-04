//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/export/picture_export.hh>
#include <tomb-excavator/formats/image/picture.hh>

namespace exporter
{
    picture_export::picture_export(std::filesystem::path opath)
    : m_opath(std::move(opath))
    {

    }
    // ----------------------------------------------------------------------------------
    void picture_export::transform(const provider::dto::picture& pic)
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
        formats::image::save_to_png(opic, m_opath);
    }
} // ns
