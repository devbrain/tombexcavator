//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_SPRITE_LOADER_HH
#define TOMBEXCAVATOR_SPRITE_LOADER_HH

#include <tomb-excavator/games/common/loaders/data_loader.hh>

namespace games::common
{
    class GAMES_COMMON_API sprite_loader : public data_loader
    {
    public:
        sprite_loader(std::string virt_name, std::string phys_name, int first_sprite_id);
    protected:
        virtual provider::file_type_t read(std::istream& is, int first_sprite_id) const = 0;
    private:
        int m_first_sprite_id;
        provider::file_type_t read(std::istream& is) const override;
    };
}

#endif //TOMBEXCAVATOR_SPRITE_LOADER_HH
