//
// Created by igor on 15/03/2021.
//

#include <tomb-excavator/provider/dto/map/map.hh>

namespace provider::dto
{
    map::map(int w, int h)
    : m_dim(w, h)
    {

    }
    // -------------------------------------------------------------------
    map::map(dimension d)
    : m_dim(std::move(d))
    {

    }
    // -------------------------------------------------------------------
    layer& map::add_layer()
    {
        return m_layers.emplace_back(m_dim);
    }
    // -------------------------------------------------------------------
    bool map::empty() const noexcept
    {
        return m_layers.empty();
    }
    // -------------------------------------------------------------------
    map::const_iterator map::begin() const
    {
        return m_layers.begin();
    }
    // -------------------------------------------------------------------
    map::const_iterator map::end() const
    {
        return m_layers.end();
    }
}