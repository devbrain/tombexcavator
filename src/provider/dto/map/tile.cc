//
// Created by igor on 15/03/2021.
//

#include <tomb-excavator/provider/dto/map/tile.hh>

namespace provider::dto
{
    tile::tile(int id)
    : m_id(id)
    {

    }
    // ----------------------------------------------------------------------
    tile::tile (int id, std::initializer_list<name_value_t> props)
    : m_id(id),
    m_props(std::move(props))
    {

    }
    // ----------------------------------------------------------------------
    int tile::get_id() const noexcept
    {
        return m_id;
    }
    // ----------------------------------------------------------------------
    bool tile::empty() const noexcept
    {
        return m_props.empty();
    }
    // ----------------------------------------------------------------------
    tile::const_iterator tile::begin() const
    {
        return m_props.begin();
    }
    // ----------------------------------------------------------------------
    tile::const_iterator tile::end() const
    {
        return m_props.end();
    }
}