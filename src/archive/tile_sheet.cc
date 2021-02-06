//
// Created by igor on 27/05/2020.
//

#include "tile_sheet.hh"
#include <algorithm>

namespace archive
{
    sprite_group::sprite_group(std::vector<coord>&& data)
    : m_sprites(std::move(data))
    {

    }
    // =======================================================================================
    tile_sheet::tile_sheet(std::vector<sprite_group>&& groups, canvas&& image)
    : m_groups(std::move(groups)), m_canvas(std::move(image))
    {

    }
    // ---------------------------------------------------------------------------------------
    bool tile_sheet::save_to_png(const std::string& oname)
    {
        return m_canvas.save_to_png(oname);
    }
    // =======================================================================================
    tile_sheet tile_sheet_builder::build()
    {
        std::size_t w = 0;
        std::size_t h = 0;

        for (const auto& g : m_sprite_groups)
        {
            std::size_t max_w = 0;
            std::size_t max_h = 0;
            for (const auto d : g)
            {
                max_w = std::max(max_w, (std::size_t)d.dim.w());
                max_h = max_h + (std::size_t)d.dim.h();
            }
            w = w + max_w;
            h = std::max(h, max_h);
        }
        std::vector<sprite_group>  groups;
        canvas pixels(w, h);
        pixels.pal() = m_palette;

        std::size_t px = 0;
        for (const auto& g : m_sprite_groups)
        {
            std::size_t max_w = 0;
            std::size_t py = 0;
            std::vector<coord> current_group;
            for (const auto d : g)
            {
                coord crd (position(px, py), d.dim);
                current_group.push_back(crd);
                max_w = std::max(max_w, (std::size_t)d.dim.w());
                const auto& img = m_sprites[d.index];
                pixels.copy(px, py, img, d.dim.w(), d.dim.h());
                py += d.dim.h();
            }
            groups.push_back(sprite_group(std::move(current_group)));
            px = px + max_w;
        }
        return tile_sheet(std::move(groups), std::move(pixels));
    }
    // ----------------------------------------------------------------------------------------------------
    tile_sheet_builder& tile_sheet_builder::add_palette(const palette& pal)
    {
        m_palette = pal;
        return *this;
    }
    // ----------------------------------------------------------------------------------------------------
    tile_sheet_builder& tile_sheet_builder::start_group()
    {
        m_current_group.clear();
        return *this;
    }
    // ----------------------------------------------------------------------------------------------------
    tile_sheet_builder& tile_sheet_builder::add_sprite(std::vector<unsigned char>&& data, int w, int h)
    {
        m_current_group.emplace_back(m_sprites.size(), w, h);
        m_sprites.push_back(std::move(data));
        return *this;
    }
    // ----------------------------------------------------------------------------------------------------
    tile_sheet_builder& tile_sheet_builder::end_group()
    {
        if (!m_current_group.empty())
        {
            m_sprite_groups.push_back(m_current_group);
        }
        return *this;
    }
    // ------------------------------------------------------------------------------------------------------
    tile_sheet_builder::sprite_data::sprite_data (std::size_t index_, int w, int h)
    : index(index_), dim(w, h)
    {

    }
} // ns archive
