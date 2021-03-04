//
// Created by igor on 23/02/2021.
//

#ifndef TOMBEXCAVATOR_PROGRAPHX_HH
#define TOMBEXCAVATOR_PROGRAPHX_HH

#include <iosfwd>
#include <tomb-excavator/games-apogee_export.h>
#include <tomb-excavator/provider/dto/video//sprite.hh>

namespace apogee
{
  [[nodiscard]] GAMES_APOGEE_API provider::dto::sprite_group load_prographx_tiles(std::istream& istream, std::size_t padding);
}

#endif //TOMBEXCAVATOR_PROGRAPHX_HH