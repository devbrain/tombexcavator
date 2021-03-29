//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_PALETTE_LOADER_HH
#define TOMBEXCAVATOR_PLAETTE_LOADER_HH

#include <tomb-excavator/games/common/loaders/single_entry_loader.hh>
#include <tomb-excavator/msvc/c4251-begin.h>

namespace games::common
{
    class GAMES_COMMON_API palette_loader : public single_entry_physical_data_loader<provider::dto::palette>
    {
    public:
        palette_loader(std::string virt_name, std::string phys_name);
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_SPRITE_LOADER_HH
