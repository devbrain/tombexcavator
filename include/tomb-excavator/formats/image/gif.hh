//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_GIF_HH
#define TOMBEXCAVATOR_GIF_HH

#include <tomb-excavator/formats_export.h>
#include "tomb-excavator/formats/image/picture.hh"

namespace formats::image
{
    FORMATS_API bool is_gif(const char* input, std::size_t input_length);
    FORMATS_API bool load_gif(const char* input, std::size_t input_length, picture& out);
}

#endif //TOMBEXCAVATOR_JPEG_HH
