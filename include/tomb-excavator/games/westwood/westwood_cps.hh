//
// Created by igor on 12/04/2021.
//

#ifndef TOMBEXCAVATOR_WESTWOOD_CPS_HH
#define TOMBEXCAVATOR_WESTWOOD_CPS_HH

#include <iosfwd>
#include <optional>

#include <tomb-excavator/export-games-westwood.h>
#include <tomb-excavator/provider/dto/video/picture.hh>

namespace games::westwood
{
    GAMES_WESTWOOD_API provider::dto::picture load_cps(std::istream &is, std::size_t entry_size,
                                                       const std::optional<provider::dto::palette>& palette);
}

#endif //TOMBEXCAVATOR_WESTWOOD_CPS_HH
