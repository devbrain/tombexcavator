//
// Created by igor on 24/02/2021.
//

#ifndef TOMBEXCAVATOR_EGA_PALETTE_HH
#define TOMBEXCAVATOR_EGA_PALETTE_HH

#include <tomb-excavator/games-common_export.h>
#include <tomb-excavator/provider/dto/video//palette.hh>

namespace games::common
{
    class GAMES_COMMON_API ega_palette : public provider::dto::palette
    {
    public:
        ega_palette();
    };
} // ns common

#endif //TOMBEXCAVATOR_EGA_PALETTE_HH
