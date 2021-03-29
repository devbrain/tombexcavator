//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_SPRITE_LOADER_HH
#define TOMBEXCAVATOR_SPRITE_LOADER_HH

#include <tomb-excavator/games/common/loaders/single_entry_loader.hh>
#include <tomb-excavator/msvc/c4251-begin.h>
namespace games::common
{
    class GAMES_COMMON_API sprite_loader : public single_entry_physical_data_loader<provider::dto::sprite_group>
    {
    public:
        using sprite_loader_t = std::function<provider::dto::sprite_group(std::istream& is, int first_sprite_id)>;
    public:
        sprite_loader(std::string virt_name, std::string phys_name, int first_sprite_id, sprite_loader_t loader);
    };
}
#include <tomb-excavator/msvc/c4251-end.h>
#endif //TOMBEXCAVATOR_SPRITE_LOADER_HH
