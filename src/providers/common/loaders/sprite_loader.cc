//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/loaders/sprite_loader.hh>

namespace games::common
{
    sprite_loader::sprite_loader(std::string virt_name, std::string phys_name, int first_sprite_id,
                                 sprite_loader_t loader)
            : single_entry_physical_data_loader<provider::dto::sprite_group>(std::move(virt_name),
                                                                             std::move(phys_name),
                                                                             [first_sprite_id, loader](
                                                                                     std::istream& is) {
                                                                                 return loader(is, first_sprite_id);
                                                                             })
    {

    }
}