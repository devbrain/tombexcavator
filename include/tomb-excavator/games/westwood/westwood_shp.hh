//
// Created by igor on 15/04/2021.
//

#ifndef TOMBEXCAVATOR_WESTWOOD_SHP_HH
#define TOMBEXCAVATOR_WESTWOOD_SHP_HH

#include <optional>
#include <cstdint>
#include <tomb-excavator/export-games-westwood.h>
#include <tomb-excavator/provider/dto/video/sprite.hh>

namespace games::westwood
{
    GAMES_WESTWOOD_API provider::dto::sprite_group load_shp(std::istream &is, std::size_t entry_size,
                                                            const provider::dto::palette& palette);
}


#endif //TOMBEXCAVATOR_WESTWOOD_SHP_HH
