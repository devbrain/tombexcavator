//
// Created by igor on 28/02/2021.
//

#include "sprite_loader.hh"
#include <tomb-excavator/games/apogee/prographx.hh>

provider::file_type_t sprite_loader::read(std::istream& is) const
{
    return apogee::load_prographx_tiles(is, 0, m_first_sprite_id);
}