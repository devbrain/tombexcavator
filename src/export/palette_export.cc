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
        auto n = pal.size();
        for (std::size_t color = 0; color < n; color++)
        {
            provider::dto::sprite& sp = sg.add(32, 32);
            sp.get_canvas().fill(static_cast<uint8_t>(color));
            sp.set_id(color);
        }
        to_png_with_ids(sg, opath);
    }
}
