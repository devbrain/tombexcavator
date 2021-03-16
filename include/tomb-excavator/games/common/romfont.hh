//
// Created by igor on 15/03/2021.
//

#ifndef TOMBEXCAVATOR_ROMFONT_HH
#define TOMBEXCAVATOR_ROMFONT_HH

#include <tomb-excavator/export-games-common.h>
#include <vector>


namespace games::common
{
    enum class font
    {
        IBM_8x8,
        IBM_8x14,
        IBM_8x16
    };

    [[nodiscard]] GAMES_COMMON_API const std::vector<std::vector<bool>>& get_glyph(font font_type, char ch);
}

#endif //TOMBEXCAVATOR_ROMFONT_HH
