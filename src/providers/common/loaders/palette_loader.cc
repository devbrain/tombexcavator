//
// Created by igor on 28/02/2021.
//

#include <tomb-excavator/games/common/loaders/palette_loader.hh>
#include <tomb-excavator/games/common/load_palette.hh>

namespace games::common
{
    palette_loader::palette_loader(std::string virt_name, std::string phys_name)
            : single_entry_physical_data_loader<provider::dto::palette>(std::move(virt_name),
                                                                             std::move(phys_name),
                                                                             [](std::istream& is) {
                                                                                return load_palette(is);
                                                                             })
    {

    }
}