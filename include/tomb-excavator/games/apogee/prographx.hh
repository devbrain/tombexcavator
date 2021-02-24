//
// Created by igor on 23/02/2021.
//

#ifndef TOMBEXCAVATOR_PROGRAPHX_HH
#define TOMBEXCAVATOR_PROGRAPHX_HH

#include <iosfwd>
#include <tombexcavator-games-apogee_export.h>
#include <tomb-excavator/games/common/tile_sheet.hh>

namespace apogee
{
  [[nodiscard]] GAMES_APOGEE_API  common::tile_sheet load_prographx_tiles(std::istream& istream, std::size_t padding);
}

#endif //TOMBEXCAVATOR_PROGRAPHX_HH
