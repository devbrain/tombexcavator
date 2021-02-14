//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_JPEG_HH
#define TOMBEXCAVATOR_JPEG_HH

#include <tombexcavator-formats_export.h>
#include "formats/image/picture.hh"

namespace formats::image
{
    FORMATS_API bool is_jpeg(const char* input, std::size_t input_length);
    FORMATS_API bool load_jpeg(const char* input, std::size_t input_length, picture& out);
}

#endif //TOMBEXCAVATOR_JPEG_HH
