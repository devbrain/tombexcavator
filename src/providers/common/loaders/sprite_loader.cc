//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/loaders/sprite_loader.hh>

namespace games::common
{
    sprite_loader::sprite_loader(std::string virt_name, std::string phys_name, int first_sprite_id)
    : data_loader(virt_name, phys_name),
    m_first_sprite_id(first_sprite_id)
    {

    }
    // ----------------------------------------------------------------------------------------------------------
    provider::file_type_t sprite_loader::read(std::istream& is) const
    {
        return read(is, m_first_sprite_id);
    }
}