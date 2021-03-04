//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/provider/dto/video/sprite.hh>

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

}
