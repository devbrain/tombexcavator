//
// Created by igor on 29/03/2021.
//

#ifndef TOMBEXCAVATOR_LOAD_PALETTE_HH
#define TOMBEXCAVATOR_LOAD_PALETTE_HH

#include <tomb-excavator/export-games-common.h>
#include <tomb-excavator/provider/dto/video//palette.hh>
#include <vector>
#include <iosfwd>

namespace games::common
{
    [[nodiscard]] GAMES_COMMON_API provider::dto::palette load_palette(const std::vector<char>& raw);
    [[nodiscard]] GAMES_COMMON_API provider::dto::palette load_palette(std::istream& is);
}


#endif //TOMBEXCAVATOR_LOAD_PALETTE_HH
