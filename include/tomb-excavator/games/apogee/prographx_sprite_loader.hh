//
// Created by igor on 28/02/2021.
//

#ifndef TOMBEXCAVATOR_APOGEE_PROGRAPHX_SPRITE_LOADER_HH
#define TOMBEXCAVATOR_APOGEE_PROGRAPHX_SPRITE_LOADER_HH

#include <string>
#include <tomb-excavator/games/common/loaders/sprite_loader.hh>
#include <tomb-excavator/games-apogee_export.h>

namespace apogee
{
    class GAMES_APOGEE_API prographx_sprite_loader : public games::common::sprite_loader
    {
    public:
        prographx_sprite_loader (std::string virt_name, std::string phys_name,
                      int first_sprite_id, std::size_t padding);

    private:
        std::size_t m_padding;
        provider::file_type_t read(std::istream& is, int first_sprite_id) const override;
    };
}

#endif //TOMBEXCAVATOR_SPRITE_LOADER_HH
