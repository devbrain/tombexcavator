//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/export/sprite_group_export.hh>
#include <tomb-excavator/export/xml_writer.hh>
#include <tomb-excavator/export/romfont.hh>
#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include "canvas_utils.hh"
#include <fstream>
#include <algorithm>
#include <sstream>
#include <map>

namespace exporter
{
    void to_png(const provider::dto::tile_sheet& ts, const std::filesystem::path& oname)
    {
        auto file = std::fstream(oname, std::ios::out | std::ios::binary);

        if (!file)
        {
            RAISE_EX("Failed to create file ", oname);
        }
        file.write(ts.png().data(), ts.png().size());
        file.close();
    }
    // -------------------------------------------------------------------------------------------
    void to_tsx(const provider::dto::tile_sheet& ts,
                const std::string& tileset_name,
                const std::filesystem::path& oname_tmx,
                const std::filesystem::path& oname_png)
    {
        to_png(ts, oname_png);
        std::ostringstream os;
        START_XML(os)
        {
            XML_PROLOG();
            XML_NODE("tileset", {"name", tileset_name},
                     {"tilewidth", ts.sprite_dim().w()}, {"tileheight", ts.sprite_dim().h()}, {"spacing", 0}, {"margins", 0}, {"columns", 1},
                     {"tilecount", ts.size()})
            {
                XML_NODE("image", {"source", oname_tmx}, {"width", ts.dim().w()}, {"height", ts.dim().h()});
            }
        }
        std::ofstream ofs(oname_tmx, std::ios::binary | std::ios::out);
        const auto s = os.str();
        ofs.write(s.c_str(), s.size());
    }
    // -------------------------------------------------------------------------------------------
    void to_png_with_ids(const provider::dto::sprite_group& sg, const std::filesystem::path& oname)
    {

        int max_width = 0;
        int max_height = 0;
        for (const auto& sprite : sg)
        {
            auto w = sprite.dim().w();
            max_width = std::max(w, max_width);

            auto h = sprite.dim().h();
            max_height = std::max(h, max_height);
        }

        const std::size_t sprites_per_row = 32;
        auto sprites_num = sg.size();
        const int max_label_width_px = 8*4;
        const int label_height_px = 16;
        const int max_item_width_px = std::max(max_label_width_px, max_width);
        const int max_item_height_px = max_height + label_height_px;

        auto num_rows = sprites_num / sprites_per_row;
        if (sprites_num % sprites_per_row != 0)
        {
            num_rows++;
        }
        int width_px = max_item_width_px*sprites_per_row;
        int height_px = num_rows*max_item_height_px;

        formats::image::picture out(width_px, height_px, true);


        int y = 0;
        std::size_t sprite_idx = 0;
        for (std::size_t r = 0; r<num_rows; r++)
        {
            int delta_y = 0;
            int x = 0;
            for (std::size_t c = 0; c<sprites_per_row; c++)
            {
                if (sprite_idx < sg.size())
                {
                    const auto& sprite = sg[sprite_idx++];
                    auto sprite_pos = draw_on_canvas(out, x, y, sprite, sg.pal());
                    auto opt_id = sprite.get_id();
                    std::ostringstream os;
                    if (opt_id)
                    {
                        os << std::setfill('.') << std::setw(4) << std::left << opt_id.value();
                    }
                    else
                    {
                        os << "....";
                    }
                    auto label_pos = draw_on_canvas(out, x, y + sprite_pos.dim().h(),
                                                    font::IBM_8x16, os.str(), {0xFF, 0xFF, 0xFF});
                    x += std::max(label_pos.dim().w(), sprite_pos.dim().w());
                    delta_y = std::max(delta_y, sprite_pos.dim().h() + label_pos.dim().h());
                }
                else
                {
                    break;
                }
            }
            y += delta_y;
        }
        formats::image::save_to_png(out, oname);
    }
}
