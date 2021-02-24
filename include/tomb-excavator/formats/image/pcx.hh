//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_PCX_HH
#define TOMBEXCAVATOR_PCX_HH

#include <tombexcavator-formats_export.h>
#include "picture.hh"

namespace formats::image
{
    FORMATS_API bool is_pcx(const char* input, std::size_t input_length);
    FORMATS_API bool load_pcx(const char* input, std::size_t input_length, picture& out);
}

#endif //TOMBEXCAVATOR_JPEG_HH
