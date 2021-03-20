//
// Created by igor on 15/03/2021.
//

#include <tomb-excavator/provider/dto/map/map.hh>

namespace provider::dto
{
    basic_map::basic_map(int w, int h)
    : m_dim(w, h)
    {

    }
    // -------------------------------------------------------------------
    basic_map::basic_map(dimension d)
    : m_dim(std::move(d))
    {

    }
    // -------------------------------------------------------------------
    layer& basic_map::add_layer()
    {
        return m_layers.emplace_back(m_dim);
    }
    // -------------------------------------------------------------------
    bool basic_map::empty() const noexcept
    {
        return m_layers.empty();
    }
    // -------------------------------------------------------------------
    basic_map::const_iterator basic_map::begin() const
    {
        return m_layers.begin();
    }
    // -------------------------------------------------------------------
    basic_map::const_iterator basic_map::end() const
    {
        return m_layers.end();
    }
}