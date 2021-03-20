//
// Created by igor on 28/02/2021.
//

#include <map>

#include <tomb-excavator/provider/dto/video/sprite.hh>
#include <tomb-excavator/formats/image/picture.hh>
#include <tomb-excavator/bsw/exceptions.hh>

namespace provider::dto
{
    sprite::sprite(int w, int h, bool is_masked)
    : m_canvas(w, h)
    {
        if (is_masked)
        {
            m_bitmask = bitmask(w, h);
        }
    }
    // ------------------------------------------------------------------------------------
    sprite::sprite(const dimension& dim, bool is_masked)
    : m_canvas(dim)
    {
        if (is_masked)
        {
            m_bitmask = bitmask(dim);
        }
    }
    // ------------------------------------------------------------------------------------
    const std::optional<bitmask>& sprite::get_bitmask() const noexcept
    {
        return m_bitmask;
    }
    // ------------------------------------------------------------------------------------
    std::optional<bitmask>& sprite::get_bitmask() noexcept
    {
        return m_bitmask;
    }
    // ------------------------------------------------------------------------------------
    const canvas& sprite::get_canvas() const noexcept
    {
        return m_canvas;
    }
    // ------------------------------------------------------------------------------------
    canvas& sprite::get_canvas() noexcept
    {
        return m_canvas;
    }
    // ------------------------------------------------------------------------------------
    const dimension& sprite::dim() const noexcept
    {
        return m_canvas.dim();
    }
    // ------------------------------------------------------------------------------------
    void sprite::set_id(int id)
    {
        m_id = id;
    }
    // ------------------------------------------------------------------------------------
    std::optional<int> sprite::get_id() const noexcept
    {
        return m_id;
    }
    // =====================================================================================
    sprite_group sprite_group::split_by_index(const sprite_group& origin, const std::vector<std::size_t>& indexes)
    {
        sprite_group res;
        res.m_pal = origin.m_pal;
        for (const auto idx : indexes)
        {
            res.m_sprites.push_back(origin.m_sprites[idx]);
        }
        return res;
    }
    // ------------------------------------------------------------------------------------
    sprite_group::iterator sprite_group::begin()
    {
        return m_sprites.begin();
    }
    // ------------------------------------------------------------------------------------
    sprite_group::iterator sprite_group::end()
    {
        return m_sprites.end();
    }
    // ------------------------------------------------------------------------------------
    sprite_group::const_iterator sprite_group::begin() const
    {
        return m_sprites.begin();
    }
    // ------------------------------------------------------------------------------------
    sprite_group::const_iterator sprite_group::end() const
    {
        return m_sprites.end();
    }
    // ------------------------------------------------------------------------------------
    sprite& sprite_group::add(int w, int h, bool is_masked)
    {
        return m_sprites.emplace_back(w, h, is_masked);
    }
    // ------------------------------------------------------------------------------------
    sprite& sprite_group::add(const dimension& dim, bool is_masked)
    {
        return m_sprites.emplace_back(dim, is_masked);
    }
    // ------------------------------------------------------------------------------------
    const palette& sprite_group::pal() const
    {
        return m_pal;
    }
    // ------------------------------------------------------------------------------------
    palette& sprite_group::pal()
    {
        return m_pal;
    }
    // ------------------------------------------------------------------------------------
    bool sprite_group::empty() const
    {
        return m_sprites.empty();
    }
    // ------------------------------------------------------------------------------------
    std::size_t sprite_group::size() const
    {
        return m_sprites.size();
    }
    // ------------------------------------------------------------------------------------
    const sprite& sprite_group::operator[] (std::size_t k) const
    {
        return m_sprites[k];
    }
    // ------------------------------------------------------------------------------------
    sprite& sprite_group::operator[] (std::size_t k)
    {
        return m_sprites[k];
    }
    // ==========================================================================================
    tile_sheet::sprite_info::sprite_info(int x, int y, int w, int h, int id)
            : m_coord({x, y}, {w,h}),
              m_id(id)
    {

    }
    // ------------------------------------------------------------------------------------------
    std::tuple<dimension,dimension> tile_sheet::eval_dim(const provider::dto::sprite_group& sg)
    {
        int ow = 0;
        int oh = 0;
        int mh = 0;
        for (const auto& sp : sg)
        {
            const auto& d  = sp.dim();
            ow = std::max(ow, d.w());
            mh = std::max(mh, d.h());
            oh += d.h();
        }
        return {{ow, oh}, {ow, mh}};
    }
    // ------------------------------------------------------------------------------------------
    static bool operator < (const dimension& a, const dimension& b)
    {
        return std::make_tuple(a.w(), a.h()) < std::make_tuple(b.w(), b.h());
    }
    // ------------------------------------------------------------------------------------------
    std::vector<tile_sheet> tile_sheet::create(const provider::dto::sprite_group& sg)
    {
        std::map<dimension, std::vector<std::size_t>> dim_map;
        for (std::size_t i=0; i<sg.size(); i++)
        {
            const auto& s = sg[i];
            dim_map[s.dim()].push_back(i);
        }
        std::vector<tile_sheet> result;
        if (dim_map.size() == 1)
        {
            result.emplace_back(sg);
        }
        else
        {
            for (const auto& [d, idx] : dim_map)
            {
                result.emplace_back(sprite_group::split_by_index(sg, idx));
            }
        }
        return result;
    }
    // ------------------------------------------------------------------------------------------
    tile_sheet::tile_sheet(const provider::dto::sprite_group& sg)
            : m_dims(eval_dim(sg))
    {
        const auto& dim = std::get<0>(m_dims);
        formats::image::picture opic(dim.w(), dim.h(), true);
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
    dimension tile_sheet::dim() const noexcept
    {
        return std::get<0>(m_dims);
    }
    // ------------------------------------------------------------------------------------------
    dimension tile_sheet::sprite_dim() const noexcept
    {
        return std::get<1>(m_dims);
    }
}
