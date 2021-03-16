//
// Created by igor on 21/02/2021.
//

#ifndef TOMBEXCAVATOR_FILE_TYPES_HH
#define TOMBEXCAVATOR_FILE_TYPES_HH

#include <variant>
#include <tuple>
#include <string>

#include <tomb-excavator/provider/dto/video/canvas.hh>
#include <tomb-excavator/provider/dto/video/palette.hh>
#include <tomb-excavator/provider/dto/video/sprite.hh>
#include <tomb-excavator/provider/dto/video/picture.hh>
#include <tomb-excavator/formats/image/picture.hh>

namespace provider
{
    using file_type_t = std::variant<dto::sprite_group,
                                     dto::picture,
                                     formats::image::picture,
                                     std::string>;
}

#endif //TOMBEXCAVATOR_FILE_TYPES_HH
