//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/export/sprite_group_export.hh>
#include <tomb-excavator/export/xml_writer.hh>
#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/bsw/exceptions.hh>
#include <fstream>
#include <algorithm>
#include <sstream>


namespace exporter
{
    tile_sheet::sprite_info::sprite_info(int x, int y, int w, int h, int id)
    : m_coord({x, y}, {w,h}),
    m_id(id)
    {

    }
    // ------------------------------------------------------------------------------------------
    provider::dto::dimension tile_sheet::eval_dim(const provider::dto::sprite_group& sg)
    {
        int ow = 0;
        int oh = 0;
        for (const auto& sp : sg)
        {
            const auto& d  = sp.dim();
            ow = std::max(ow, d.w());
            oh += d.h();
        }
        return {ow, oh};
    }
    // ------------------------------------------------------------------------------------------
    tile_sheet::tile_sheet(const provider::dto::sprite_group& sg)
    : m_dim(eval_dim(sg))
    {
        formats::image::picture opic(m_dim.w(), m_dim.h(), true);
        int oy =0;
        const auto& pal = sg.pal();
        for (const auto& sp : sg)
        {
            const auto& d = sp.dim();
            const auto& canvas = sp.get_canvas();
            const auto& bm = sp.get_bitmask();
            auto id = sp.get_id();
            m_sprites.emplace_back(0, oy, d.w(), d.h(), id ? *id : -1);

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
                    opic.put(x, oy, r, g, b, a);
                }
                oy++;
            }
        }
        if (!formats::image::save_to_png(opic, m_png))
        {
            RAISE_EX("Failed to convert spritesheet to PNG");
        }
    }
    // ------------------------------------------------------------------------------------------
    bool tile_sheet::empty() const noexcept
    {
        return m_sprites.empty();
    }
    // ------------------------------------------------------------------------------------------
    std::size_t tile_sheet::size() const noexcept
    {
        return m_sprites.size();
    }
    // ------------------------------------------------------------------------------------------
    tile_sheet::sprites_vec_t::const_iterator tile_sheet::begin() const
    {
        return m_sprites.begin();
    }
    // ------------------------------------------------------------------------------------------
    tile_sheet::sprites_vec_t::const_iterator tile_sheet::end() const
    {
        return m_sprites.end();
    }
    // ------------------------------------------------------------------------------------------
    const tile_sheet::png_t& tile_sheet::png() const noexcept
    {
        return m_png;
    }
    // ------------------------------------------------------------------------------------------
    provider::dto::dimension tile_sheet::dim() const noexcept
    {
        return m_dim;
    }
    // ===========================================================================================
    void to_png(const tile_sheet& ts, const std::filesystem::path& oname)
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
    void to_tsx(const tile_sheet& ts,
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
                     {"tilewidth", 16}, {"tileheight", 16}, {"spacing", 0}, {"margins", 0}, {"columns", 1},
                     {"tilecount", ts.size()})
            {
                XML_NODE("image", {"source", oname_tmx}, {"width", 16}, {"height", 16*ts.size()});
            }
        }
        std::ofstream ofs(oname_tmx, std::ios::binary | std::ios::out);
        const auto s = os.str();
        ofs.write(s.c_str(), s.size());
    }
}
