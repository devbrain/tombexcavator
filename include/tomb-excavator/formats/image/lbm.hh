//
// Created by igor on 14/02/2021.
//

#ifndef TOMBEXCAVATOR_LBM_HH
#define TOMBEXCAVATOR_LBM_HH

#include <tomb-excavator/export-formats.h>
#include "tomb-excavator/formats/image/picture.hh"

namespace formats::image
{
    FORMATS_API bool is_lbm(const char* input, std::size_t input_length);
    FORMATS_API bool load_lbm(const char* input, std::size_t input_length, picture& out);
}

#endif //TOMBEXCAVATOR_JPEG_HH
