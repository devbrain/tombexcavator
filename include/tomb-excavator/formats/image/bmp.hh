//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_BMP_HH
#define TOMBEXCAVATOR_BMP_HH

#include <tomb-excavator/export-formats.h>
#include "tomb-excavator/formats/image/picture.hh"

namespace formats::image
{
    FORMATS_API bool is_bmp(const char* input, std::size_t input_length);
    FORMATS_API bool load_bmp(const char* input, std::size_t input_length, picture& out);
}

#endif //TOMBEXCAVATOR_JPEG_HH
