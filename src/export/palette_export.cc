//
// Created by igor on 29/03/2021.
//

#include <tomb-excavator/export/palette_export.hh>
#include <tomb-excavator/export/sprite_group_export.hh>

namespace exporter
{
    void to_png(const provider::dto::palette& pal, const std::filesystem::path& opath)
    {
        provider::dto::sprite_group sg;
        sg.pal() = pal;
        for (std::size_t color = 0; color < pal.size(); color++)
        {
            provider::dto::sprite sp = sg.add(32, 32);
            sp.set_id(color);
            sp.get_canvas().fill(color);
        }
        to_png_with_ids(sg, opath);
    }
}
