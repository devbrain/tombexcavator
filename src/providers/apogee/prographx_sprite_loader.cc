//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/apogee/prographx_sprite_loader.hh>
#include <tomb-excavator/games/apogee/prographx.hh>

namespace apogee
{
    prographx_sprite_loader::prographx_sprite_loader(std::string virt_name, std::string phys_name,
                                                     int first_sprite_id, std::size_t padding)
            : games::common::sprite_loader(virt_name,
                                           phys_name,
                                           first_sprite_id,
                                           [padding](std::istream& is, int spid) {return apogee::load_prographx_tiles(is, padding, spid);}
                                           )

    {

    }
}