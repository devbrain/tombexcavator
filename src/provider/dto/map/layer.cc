//
// Created by igor on 15/03/2021.
//

#include <tomb-excavator/provider/dto/map/layer.hh>

namespace provider::dto
{
    layer::layer (int w, int h)
    : m_dim(w, h)
    {
        m_tiles.resize(w*h, tile{-1});
    }
    // ------------------------------------------------------------------
    layer::layer (dimension dims)
    : m_dim(std::move(dims))
    {
        m_tiles.resize(m_dim.w()*m_dim.h(), tile{-1});
    }
    // ------------------------------------------------------------------
    dimension layer::dim() const noexcept
    {
        return m_dim;
    }
    // ------------------------------------------------------------------
    const tile& layer::get_tile(int x, int y) const
    {
        if (x >= m_dim.w())
        {
            RAISE_EX(x, ">=", m_dim.w());
        }
        if ( y >= m_dim.h())
        {
            RAISE_EX(y, ">=", m_dim.h());
        }
        return m_tiles[y*m_dim.w() + x];
    }
}
